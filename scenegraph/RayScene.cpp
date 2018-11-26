#include "RayScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"
#include "gl/textures/TextureParametersBuilder.h"
#include <glm/gtx/transform.hpp>
#include <QThreadPool>

RayScene::RayScene(Scene &scene) :
    Scene(scene){
    // TODO [INTERSECT]
    // Remember that any pointers or OpenGL objects (e.g. texture IDs) will
    // be deleted when the old scene is deleted (assuming you are managing
    // all your memory properly to prevent memory leaks).  As a result, you
    // may need to re-allocate some things here.
    loadTextures();
    loadShapes();
    loadKDTree();
}

RayScene::~RayScene(){
    delete m_master;
}

void RayScene::loadTextures(){
    for (CS123TransformPrimitive &transPrim : m_transPrims) {
        loadMapData(transPrim.primitive.material);
    }
}

void RayScene::loadShapes(){
    m_cone     = std::make_unique<ImplicitCone>();
    m_cube     = std::make_unique<ImplicitCube>();
    m_sphere   = std::make_unique<ImplicitSphere>();
    m_cylinder = std::make_unique<ImplicitCylinder>();
    m_torus    = std::make_unique<ImplicitTorus>();
}

void RayScene::loadMapData(CS123SceneMaterial& mat){
    if (!mat.textureMap.isUsed || mat.textureMap.filename.empty())
        return;

    if (m_textures.find(mat.textureMap.filename) != m_textures.end())
        return;

    if (m_texture_images.find(mat.textureMap.filename) != m_texture_images.end())
        return;

    QImage img(mat.textureMap.filename.data());
    img = img.convertToFormat(QImage::Format_RGB32);

    if (img.isNull())
        return;

    img = img.mirrored(false, true);

    m_texture_images.emplace(mat.textureMap.filename, img);
}

void RayScene::loadKDTree(){
    float acc_surface = 0, surface, xMin = FLT_MAX, xMax = -FLT_MAX, yMin = FLT_MAX, yMax = -FLT_MAX, zMin = FLT_MAX, zMax = -FLT_MAX;
    BoundingBox bbox;
    ImplicitShape * shape;
    KDTreePrimitive kdprim;

    for (CS123TransformPrimitive& transprim : m_transPrims) {
        shape = getShapePointer(transprim.primitive.type);
        shape->setTransform(transprim.transform, transprim.transformInv);
        surface = shape->surfaceArea();
        acc_surface += surface;
        bbox    = shape->boundingBox();
        returnShapePointer(transprim.primitive.type, shape);
        xMin = std::min(xMin, bbox.xMin);
        xMax = std::max(xMax, bbox.xMax);
        yMin = std::min(yMin, bbox.yMin);
        yMax = std::max(yMax, bbox.yMax);
        zMin = std::min(zMin, bbox.zMin);
        zMax = std::max(zMax, bbox.zMax);

        kdprim.bbox      = bbox;
        kdprim.surface   = surface;
        kdprim.transprim = &transprim;
        m_kd_root.primitives.push_back(kdprim);
    }
    m_kd_root.bbox = BoundingBox(xMin, xMax, yMin, yMax, zMin, zMax);
    m_kd_root.surface = acc_surface;

    split(&m_kd_root);
}

void RayScene::split(KDTreeNode * root){
    float costNotSplit = root->primitives.size(), costX = FLT_MAX, costY = FLT_MAX, costZ = FLT_MAX, splitX, splitY,
      splitZ;
    float surfaceLX, surfaceRX, surfaceLY, surfaceRY, surfaceLZ, surfaceRZ;

    trySplit(root, costX, splitX, surfaceLX, surfaceRX, AXIS_X);
    trySplit(root, costY, splitY, surfaceLY, surfaceRY, AXIS_Y);
    trySplit(root, costZ, splitZ, surfaceLZ, surfaceRZ, AXIS_Z);

    if (costNotSplit < costX && costNotSplit < costY && costNotSplit < costZ)
        return;

    root->left  = new KDTreeNode();
    root->right = new KDTreeNode();

    if (costX <= costY && costX <= costZ) {
        // Split along X-axis
        root->left->surface  = surfaceLX;
        root->right->surface = surfaceRX;
        root->left->bbox     = BoundingBox(root->bbox.xMin, splitX,
            root->bbox.yMin, root->bbox.yMax,
            root->bbox.zMin, root->bbox.zMax);
        root->right->bbox = BoundingBox(splitX, root->bbox.xMax,
            root->bbox.yMin, root->bbox.yMax,
            root->bbox.zMin, root->bbox.zMax);

        for (KDTreePrimitive& prim : root->primitives) {
            if (prim.bbox.xMin <= splitX)
                root->left->primitives.push_back(prim);
            if (prim.bbox.xMax > splitX)
                root->right->primitives.push_back(prim);
        }
    } else if (costY <= costX && costY <= costZ) {
        // Split along Y-axis
        root->left->surface  = surfaceLY;
        root->right->surface = surfaceRY;
        root->left->bbox     = BoundingBox(root->bbox.xMin, root->bbox.xMax,
            root->bbox.yMin, splitY,
            root->bbox.zMin, root->bbox.zMax);
        root->right->bbox = BoundingBox(root->bbox.xMin, root->bbox.xMax,
            splitY, root->bbox.yMax,
            root->bbox.zMin, root->bbox.zMax);

        for (KDTreePrimitive& prim : root->primitives) {
            if (prim.bbox.yMin <= splitY)
                root->left->primitives.push_back(prim);
            if (prim.bbox.yMax > splitY)
                root->right->primitives.push_back(prim);
        }
    } else {
        // Split along Z-axis
        root->left->surface  = surfaceLZ;
        root->right->surface = surfaceRZ;
        root->left->bbox     = BoundingBox(root->bbox.xMin, root->bbox.xMax,
            root->bbox.yMin, root->bbox.yMax,
            root->bbox.zMin, splitZ);
        root->right->bbox = BoundingBox(root->bbox.xMin, root->bbox.xMax,
            root->bbox.yMin, root->bbox.yMax,
            splitZ, root->bbox.zMax);

        for (KDTreePrimitive& prim : root->primitives) {
            if (prim.bbox.zMin <= splitZ)
                root->left->primitives.push_back(prim);
            if (prim.bbox.zMax > splitZ)
                root->right->primitives.push_back(prim);
        }
    }

    // Don't need primitives anymore
    root->primitives.clear();

    split(root->left);
    split(root->right);
} // RayScene::split

void RayScene::trySplit(KDTreeNode * root, float& mincost, float& split, float& surfaceL, float& surfaceR, Axis axis){
    float cost;
    float surfaceLeft = 0, surfaceRight = root->surface;
    float surfaceLeftBase = 0, surfaceRightBase = root->surface;
    float weightLeft, weightRight;
    int countLeft = 0, countRight = root->primitives.size();

    switch (axis) {
        case AXIS_X:
            std::sort(root->primitives.begin(), root->primitives.end(), less_than_x_key());
            break;
        case AXIS_Y:
            std::sort(root->primitives.begin(), root->primitives.end(), less_than_y_key());
            break;
        case AXIS_Z:
            std::sort(root->primitives.begin(), root->primitives.end(), less_than_z_key());
            break;
    }

    for (unsigned int i = 0; i < root->primitives.size() - 1; i++) {
        surfaceLeftBase  += root->primitives[i].surface;
        surfaceRightBase -= root->primitives[i].surface;

        surfaceLeft = surfaceLeftBase;
        countLeft++;

        // Handle intersected objects
        for (unsigned int j = i + 1; j < root->primitives.size(); j++) {
            bool within = axis == AXIS_X ? root->primitives[j].bbox.xMin <= root->primitives[i].bbox.xMax :
              (axis == AXIS_Y ? root->primitives[j].bbox.yMin <= root->primitives[i].bbox.yMax :
              root->primitives[j].bbox.zMin <= root->primitives[i].bbox.zMax);
            if (within) {
                surfaceLeft += root->primitives[j].surface;
                countLeft++;
            } else {
                break;
            }
        }

        surfaceRight = surfaceRightBase;
        countRight--;

        weightLeft  = surfaceLeft / root->surface;
        weightRight = surfaceRight / root->surface;
        cost        = weightLeft * countLeft + weightRight * countRight;

        if (cost < mincost) {
            mincost = cost;
            split   = axis == AXIS_X ? root->primitives[i].bbox.xMax :
              (axis == AXIS_Y ? root->primitives[i].bbox.yMax :
              root->primitives[i].bbox.zMax);
            surfaceL = surfaceLeft;
            surfaceR = surfaceRight;
        }
    }
} // RayScene::trySplit

void RayScene::render(SupportCanvas2D * canvas, Camera * camera, int width, int height){
    BGRA * data = canvas->data();

    loadCameraMatrices(camera);

    m_master = new RayTraceMaster(this, width, height, data);
    m_master->start();
}

void RayScene::loadCameraMatrices(Camera * camera){
    m_view     = camera->getViewMatrix();
    m_scale    = camera->getScaleMatrix();
    m_viewInv  = glm::inverse(m_view);
    m_scaleInv = glm::inverse(m_scale);
}

void RayScene::rayTrace(float row, float col, int width, int height, BGRA& bgra) {
    // Generate ray in camera space, and then transform it into world space
    Ray ray;

    glm::vec4 filmPixelPos = getFilmPixelPosition(row, col, width, height);
    filmPixelPos = m_viewInv * m_scaleInv * filmPixelPos;
    ray.start    = m_viewInv * glm::vec4(0, 0, 0, 1);
    ray.delta    = glm::normalize(filmPixelPos - ray.start);

    rayTraceImpl(ray, bgra, glm::vec4(1), 0);
} // RayScene::rayTrace

void RayScene::rayTraceImpl(Ray& ray, BGRA& bgra, glm::vec4 reflectCoef, int recursionLevel) {
    // Try to find an intersection using kd-tree
    Intersect intersect;
    kdTreeIntersect(&m_kd_root, ray, intersect);
    //    naiveIntersect(ray, intersect);

    if (intersect.miss) {
        // DEBUG
        return;
    }

    // Get normal
    glm::vec4 normal = calcNormal(intersect);

    // Compute illumination
    BGRA delta_bgra;
    delta_bgra = calcLight(intersect, normal);

    // Add delta
    bgra.r = glm::clamp(bgra.r + static_cast<int>(glm::round(delta_bgra.r * reflectCoef.r)), 0, 255);
    bgra.g = glm::clamp(bgra.g + static_cast<int>(glm::round(delta_bgra.g * reflectCoef.g)), 0, 255);
    bgra.b = glm::clamp(bgra.b + static_cast<int>(glm::round(delta_bgra.b * reflectCoef.b)), 0, 255);

    // Update reflectCoef
    reflectCoef *= intersect.transprim->primitive.material.cReflective;

    if (recursionLevel < MAX_RECURSION && ((reflectCoef.r + reflectCoef.g + reflectCoef.b) >= MIN_REFLECT)) {
        // Compute the reflected ray
        Ray ref_ray;
        ref_ray.start = intersect.pos + RAY_OFFSET * normal;
        ref_ray.delta = glm::normalize(glm::reflect(ray.delta, normal));

        rayTraceImpl(ref_ray, bgra, reflectCoef, recursionLevel + 1);
    }
}

glm::vec4 RayScene::calcNormal(Intersect& intersect){
    CS123TransformPrimitive * transprim = intersect.transprim;
    ImplicitShape * shape = getShapePointer(transprim->primitive.type);

    glm::mat4x4 model    = transprim->transform;
    glm::mat4x4 modelInv = transprim->transformInv;
    shape->setTransform(transprim->transform, transprim->transformInv);
    glm::vec4 normal = shape->normal(intersect);
    returnShapePointer(transprim->primitive.type, shape);
    return normal;
}

BGRA RayScene::calcLight(Intersect& intersect, glm::vec4 normal){
    CS123TransformPrimitive * transprim = intersect.transprim;

    glm::vec4 final (0), ambient, diffuse, specular;
    ambient  = transprim->primitive.material.cAmbient;
    // diffuse  = transprim->primitive.material.cDiffuse;
    diffuse = getDiffuse(intersect);
    specular = transprim->primitive.material.cSpecular;
    float shininess = transprim->primitive.material.shininess;

    glm::vec4 position_worldSpace = intersect.pos;
    glm::mat4x4 modelInv        = transprim->transformInv;
    glm::vec4 normal_worldSpace =
      glm::vec4(glm::normalize(glm::mat3(glm::transpose(modelInv)) * glm::vec3(normal)), 0);

    final = ambient;
    glm::vec4 lightColor;
    glm::vec4 vertexToLight;
    glm::vec4 vertexToEye = glm::normalize(m_viewInv * glm::vec4(glm::vec3(-(m_view * intersect.pos)), 0));
    glm::vec4 lightReflect;
    float diffuseIntensity, specularIntensity;

    for (CS123SceneLightData& light : m_lights) {
        switch (light.type) {
            case LightType::LIGHT_POINT:
                vertexToLight = glm::normalize(glm::vec4(glm::vec3(light.pos), 1) - position_worldSpace);
                break;
            case LightType::LIGHT_DIRECTIONAL:
                vertexToLight = glm::normalize(glm::vec4(-glm::vec3(light.dir), 0));
                break;
            case LightType::LIGHT_SPOT:
                perror("Unsupported light type.");
                exit(1);
                break;
            case LightType::LIGHT_AREA:
                perror("Unsupported light type.");
                exit(1);
                break;
        }

        lightColor       = light.color;
        diffuseIntensity = glm::max(0.f, glm::dot(vertexToLight, normal_worldSpace));
        final += glm::max(glm::vec4(0), lightColor * diffuse * diffuseIntensity);

        lightReflect      = glm::normalize(glm::reflect(-vertexToLight, normal_worldSpace));
        specularIntensity = glm::pow(glm::max(0.0f, glm::dot(vertexToEye, lightReflect)), shininess);
        final += glm::max(glm::vec4(0), lightColor * specular * specularIntensity);
    }

    BGRA bgra;
    bgra.r = glm::clamp(final.r, 0.f, 1.f) * 255;
    bgra.g = glm::clamp(final.g, 0.f, 1.f) * 255;
    bgra.b = glm::clamp(final.b, 0.f, 1.f) * 255;
    bgra.a = 255;
    return bgra;
} // RayScene::calcLight

glm::vec4 RayScene::getDiffuse(Intersect& intersect) {

    CS123TransformPrimitive * transprim = intersect.transprim;

    // No texture
    if (!transprim->primitive.material.textureMap.isUsed ||
         transprim->primitive.material.textureMap.filename.empty() ||
         m_texture_images.find(transprim->primitive.material.textureMap.filename) == m_texture_images.end())
        return transprim->primitive.material.cDiffuse;

    // Get UV
    ImplicitShape *shape;
    glm::vec4 diffuse;
    QImage texture;
    glm::vec2 uv;
    float blend;
    QRgb pixel;
    BGRA bgra;
    int w, h;

    shape = getShapePointer(transprim->primitive.type);
    shape->setTransform(transprim->transform, transprim->transformInv);
    uv = shape->getUV(intersect,
                      transprim->primitive.material.textureMap.repeatU,
                      transprim->primitive.material.textureMap.repeatV);
    returnShapePointer(transprim->primitive.type, shape);

    texture = m_texture_images[transprim->primitive.material.textureMap.filename];
    w = texture.width();
    h = texture.height();
    pixel = texture.pixel(glm::floor(uv.x * w), glm::floor(uv.y * h));
    bgra = *reinterpret_cast<BGRA*>(&pixel);
    diffuse = glm::vec4(bgra.r / 255.f, bgra.g / 255.f, bgra.b / 255.f, 1.0f);
    blend = transprim->primitive.material.blend;
    diffuse = glm::mix(transprim->primitive.material.cDiffuse, diffuse, blend);
    return diffuse;
}

ImplicitShape * RayScene::getShapePointer(PrimitiveType type){
    ImplicitShape * shape;

    switch (type) {
        case PrimitiveType::PRIMITIVE_CUBE:
            m_cubemtx.lock();
            shape = m_cube.release();
            break;

        case PrimitiveType::PRIMITIVE_CONE:
            m_conemtx.lock();
            shape = m_cone.release();
            break;

        case PrimitiveType::PRIMITIVE_CYLINDER:
            m_cylindermtx.lock();
            shape = m_cylinder.release();
            break;

        case PrimitiveType::PRIMITIVE_TORUS:
            m_torusmtx.lock();
            shape = m_torus.release();
            break;

        case PrimitiveType::PRIMITIVE_SPHERE:
            m_spheremtx.lock();
            shape = m_sphere.release();
            break;

        default:
            perror("Unsupported shape.");
            exit(1);
    }

    return shape;
} // RayScene::getShapePointer

void RayScene::returnShapePointer(PrimitiveType type, ImplicitShape * shape){
    switch (type) {
        case PrimitiveType::PRIMITIVE_CUBE:
            m_cube.reset(static_cast<ImplicitCube *>(shape));
            m_cubemtx.unlock();
            break;

        case PrimitiveType::PRIMITIVE_CONE:
            m_cone.reset(static_cast<ImplicitCone *>(shape));
            m_conemtx.unlock();
            break;

        case PrimitiveType::PRIMITIVE_CYLINDER:
            m_cylinder.reset(static_cast<ImplicitCylinder *>(shape));
            m_cylindermtx.unlock();
            break;

        case PrimitiveType::PRIMITIVE_TORUS:
            m_torus.reset(static_cast<ImplicitTorus *>(shape));
            m_torusmtx.unlock();
            break;

        case PrimitiveType::PRIMITIVE_SPHERE:
            m_sphere.reset(static_cast<ImplicitSphere *>(shape));
            m_spheremtx.unlock();
            break;

        default:
            perror("Unsupported shape.");
            exit(1);
    }
}

glm::vec4 RayScene::getFilmPixelPosition(float row, float col, int width, int height){
    return glm::vec4(static_cast<float>(2 * col - width) / static_cast<float>(width),
             static_cast<float>(height - 2 * row) / static_cast<float>(height),
             -1,
             1);
}

void RayScene::naiveIntersect(Ray& ray, Intersect& intersect){
    ImplicitShape * shape;
    Intersect bestItsct, itsct;

    for (CS123TransformPrimitive& transprim : m_transPrims) {
        shape = getShapePointer(transprim.primitive.type);
        shape->setTransform(transprim.transform, transprim.transformInv);
        itsct = shape->intersect(ray);
        returnShapePointer(transprim.primitive.type, shape);
        if (!itsct.miss) {
            if (itsct.t < bestItsct.t) {
                bestItsct = itsct;
                bestItsct.transprim = &transprim;
            }
        }
    }

    intersect = bestItsct;
}

void RayScene::kdTreeIntersect(KDTreeNode * root, Ray& ray, Intersect& intersect){
    // Treat each bounding box as an ImplicitCube, compute the intersectons with both of them, and take the smaller one (if any)
    ImplicitCube cubeLeft, cubeRight;
    KDTreeNode * left = root->left, * right = root->right;

    if (left && right) {
        // If the current root is not a leaf node
        glm::mat4x4 leftTransform     = left->bbox.transform;
        glm::mat4x4 leftTransformInv  = left->bbox.transformInv;
        glm::mat4x4 rightTransform    = right->bbox.transform;
        glm::mat4x4 rightTransformInv = right->bbox.transformInv;

        cubeLeft.setTransform(leftTransform, leftTransformInv);
        cubeRight.setTransform(rightTransform, rightTransformInv);

        Intersect lIntersect = cubeLeft.intersect(ray);
        Intersect rIntersect = cubeRight.intersect(ray);

        if (lIntersect.miss && rIntersect.miss) {
            // Miss both children
            return;
        } else if (lIntersect.miss) {
            // Only intersect with right children's bounding box
            kdTreeIntersect(right, ray, intersect);
        } else if (rIntersect.miss) {
            // Only intersect with left children's bounding box
            kdTreeIntersect(left, ray, intersect);
        } else {
            // Intersect with both children
            Intersect lInt, rInt;
            kdTreeIntersect(left, ray, lInt);
            kdTreeIntersect(right, ray, rInt);
            if (lInt.miss && rInt.miss)
                return;
            else if (lInt.miss)
                intersect = rInt;
            else if (rInt.miss)
                intersect = lInt;
            else
                intersect = lInt.t < rInt.t ? lInt : rInt;
        }
    } else {
        // If it is a leaf node
        Intersect itsct;
        ImplicitShape * shape;
        CS123TransformPrimitive * transprim;
        for (KDTreePrimitive& kdprim : root->primitives) {
            transprim = kdprim.transprim;
            shape     = getShapePointer(transprim->primitive.type);
            shape->setTransform(transprim->transform, transprim->transformInv);
            itsct = shape->intersect(ray);
            itsct.transprim = transprim;
            returnShapePointer(transprim->primitive.type, shape);

            if (!itsct.miss) {
                intersect = intersect.miss ? itsct : (intersect.t > itsct.t ? itsct : intersect);
            }
        }
    }
} // RayScene::kdTreeIntersect

glm::mat4x4 RayScene::boundingBoxToTransform(BoundingBox& bbox){
    float xc = (bbox.xMax + bbox.xMin) / 2, yc = (bbox.yMax + bbox.yMin) / 2, zc = (bbox.zMax + bbox.zMin) / 2;
    float xs = bbox.xMax - bbox.xMin, ys = bbox.yMax - bbox.yMin, zs = bbox.zMax - bbox.zMin;

    glm::mat4x4 mat = glm::scale(glm::vec3(xs, ys, zs)) * glm::translate(glm::vec3(xc, yc, zc));
    return mat;
}
