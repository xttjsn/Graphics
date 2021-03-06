#include "RayScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"
#include "gl/textures/TextureParametersBuilder.h"
#include "filters/filterutils.h"
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

    trySplit(root, costX, splitX, AXIS_X);
    trySplit(root, costY, splitY, AXIS_Y);
    trySplit(root, costZ, splitZ, AXIS_Z);

    if (costNotSplit < costX && costNotSplit < costY && costNotSplit < costZ)
        return;

    root->left  = new KDTreeNode();
    root->right = new KDTreeNode();

    if (costX <= costY && costX <= costZ) {
        // Split along X-axis
        root->left->surface  = 0.f;
        root->right->surface = 0.f;
        root->left->bbox     = BoundingBox(root->bbox.xMin, splitX,
            root->bbox.yMin, root->bbox.yMax,
            root->bbox.zMin, root->bbox.zMax);
        root->right->bbox = BoundingBox(splitX, root->bbox.xMax,
            root->bbox.yMin, root->bbox.yMax,
            root->bbox.zMin, root->bbox.zMax);

        for (KDTreePrimitive& prim : root->primitives) {
            if (prim.bbox.xMin <= splitX) {
                root->left->primitives.push_back(prim);
                root->left->surface += prim.surface;
            }
            if (prim.bbox.xMax > splitX) {
                root->right->primitives.push_back(prim);
                root->right->surface += prim.surface;
            }
        }
    } else if (costY <= costX && costY <= costZ) {
        // Split along Y-axis
        root->left->surface  = 0.f;
        root->right->surface = 0.f;
        root->left->bbox     = BoundingBox(root->bbox.xMin, root->bbox.xMax,
            root->bbox.yMin, splitY,
            root->bbox.zMin, root->bbox.zMax);
        root->right->bbox = BoundingBox(root->bbox.xMin, root->bbox.xMax,
            splitY, root->bbox.yMax,
            root->bbox.zMin, root->bbox.zMax);

        for (KDTreePrimitive& prim : root->primitives) {
            if (prim.bbox.yMin <= splitY) {
                root->left->primitives.push_back(prim);
                root->left->surface += prim.surface;
            }
            if (prim.bbox.yMax > splitY) {
                root->right->primitives.push_back(prim);
                root->right->surface += prim.surface;
            }
        }
    } else {
        // Split along Z-axis
        root->left->surface  = 0.f;
        root->right->surface = 0.f;
        root->left->bbox     = BoundingBox(root->bbox.xMin, root->bbox.xMax,
            root->bbox.yMin, root->bbox.yMax,
            root->bbox.zMin, splitZ);
        root->right->bbox = BoundingBox(root->bbox.xMin, root->bbox.xMax,
            root->bbox.yMin, root->bbox.yMax,
            splitZ, root->bbox.zMax);

        for (KDTreePrimitive& prim : root->primitives) {
            if (prim.bbox.zMin <= splitZ) {
                root->left->primitives.push_back(prim);
                root->left->surface += prim.surface;
            }
            if (prim.bbox.zMax > splitZ) {
                root->right->primitives.push_back(prim);
                root->right->surface += prim.surface;
            }
        }
    }

    // Don't need primitives anymore
    root->primitives.clear();

    split(root->left);
    split(root->right);
} // RayScene::split

void RayScene::trySplit(KDTreeNode * root, float& mincost, float& split, Axis axis){
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
        }
    }
} // RayScene::trySplit

void RayScene::render(SupportCanvas2D * canvas, Camera * camera){
    loadCameraMatrices(camera);

    m_canvas = canvas;
    if (settings.useMultiThreading) {
        m_master = std::make_unique<RayTraceMaster>(this, canvas);
        m_master->start();
    } else {
        BGRA bgra, *data = canvas->data();
        int h = canvas->height(), w = canvas->width();
        for (int row = 0; row < h; row++)  {
            for (int col = 0; col < w; col++) {

                if (settings.useSuperSampling && settings.numSuperSamples > 1)
                    bgra = vec2bgra(superSampleRayTrace(row, col, 1));
                else
                    bgra = vec2bgra(rayTrace(row, col));

                *(data + row * w + col) = bgra;
                bgra = BGRA(0,0,0,255);
            }
            canvas->repaint();
        }
    }
}

void RayScene::loadCameraMatrices(Camera * camera){
    m_view     = camera->getViewMatrix();
    m_scale    = camera->getScaleMatrix();
    m_viewInv  = glm::inverse(m_view);
    m_scaleInv = glm::inverse(m_scale);
}

glm::vec4 RayScene::rayTrace(float row, float col) {
    // Generate ray in camera space, and then transform it into world space
    Ray ray;

    glm::vec4 filmPixelPos = getFilmPixelPosition(row, col, m_canvas->width(), m_canvas->height());
    filmPixelPos = m_viewInv * m_scaleInv * filmPixelPos;
    ray.start    = m_viewInv * glm::vec4(0, 0, 0, 1);
    ray.delta    = glm::normalize(filmPixelPos - ray.start);

    glm::vec4 color = rayTraceImpl(ray, 0);

    return color;
} // RayScene::rayTrace

glm::vec4 RayScene::superSampleRayTrace(float row, float col, int samplingLevel) {

    // left up, left down, right up, right down
    float step = glm::pow(0.5f, static_cast<float>(samplingLevel));
    float lu_row = row - step, lu_col = col - step;
    float ld_row = row + step, ld_col = col - step;
    float ru_row = row - step, ru_col = col + step;
    float rd_row = row + step, rd_col = col + step;

    glm::vec4 center = superSampleRayTraceImpl(row, col);
    glm::vec4 lu = superSampleRayTraceImpl(lu_row, lu_col);
    glm::vec4 ld = superSampleRayTraceImpl(ld_row, ld_col);
    glm::vec4 ru = superSampleRayTraceImpl(ru_row, ru_col);
    glm::vec4 rd = superSampleRayTraceImpl(rd_row, rd_col);

    m_color_map[std::make_pair(row, col)] = CENTER_WEIGHT * center +
                                            SUB_WEIGHT * lu +
                                            SUB_WEIGHT * ld +
                                            SUB_WEIGHT * ru +
                                            SUB_WEIGHT * rd;


    if (samplingLevel < settings.numSuperSamples)
        return m_color_map[std::make_pair(row, col)];

    if (colorVariation(center, lu) > MAX_VARIATION)
        lu = superSampleRayTrace(row - step / 2.f, col - step / 2.f, samplingLevel + 1);

    if (colorVariation(center, ld) > MAX_VARIATION)
        ld = superSampleRayTrace(row + step / 2.f, col - step / 2.f, samplingLevel + 1);

    if (colorVariation(center, ru) > MAX_VARIATION)
        ru = superSampleRayTrace(row - step / 2.f, col + step / 2.f, samplingLevel + 1);

    if (colorVariation(center, rd) > MAX_VARIATION)
        rd = superSampleRayTrace(row + step / 2.f, col + step / 2.f, samplingLevel + 1);

    m_color_map[std::make_pair(row, col)] = CENTER_WEIGHT * center +
                                            SUB_WEIGHT * lu +
                                            SUB_WEIGHT * ld +
                                            SUB_WEIGHT * ru +
                                            SUB_WEIGHT * rd;

    return m_color_map[std::make_pair(row, col)];
}

glm::vec4 RayScene::rayTraceImpl(Ray& ray, int recursionLevel) {
    // Try to find an intersection using kd-tree
    Intersect intersect;
    if (settings.useKDTree)
        kdTreeIntersect(&m_kd_root, ray, intersect);
    else
        naiveIntersect(ray, intersect);

    if (intersect.miss) {
        return glm::vec4(0);
    }

    // Compute illumination
    glm::vec4 color = calcLight(ray, intersect);

    /****************** Refraction ********************/
    // Get refraction coefficient
    glm::vec4 transparent_coef = intersect.transprim->primitive.material.cTransparent;

    if (transparent_coef != glm::vec4(0) && settings.useRefraction && recursionLevel < MAX_RECURSION) {
        // We check for refraction only if the material is transparent
        Ray refract_ray;
        if (getRefractRay(ray, intersect, refract_ray, 1.0f / intersect.transprim->primitive.material.ior)) {
            // Not rotal reflection
            color += rayTraceImpl(refract_ray, recursionLevel + 1) * transparent_coef;
        }
    }

    /******************* Reflection **********************/
    // Get reflection coefficient
    glm::vec4 reflect_coef = intersect.transprim->primitive.material.cReflective;

    if (settings.useReflection && recursionLevel < MAX_RECURSION && reflect_coef != glm::vec4(0)) {
        // Compute the reflected ray
        Ray ref_ray;
        ref_ray.delta = glm::normalize(glm::reflect(ray.delta, intersect.norm_worldSpace));
        ref_ray.start = intersect.pos_worldSpace + RAY_OFFSET * ref_ray.delta;

        color += rayTraceImpl(ref_ray, recursionLevel + 1) * reflect_coef * m_global.ks;
    }


    return color;
}

glm::vec4 RayScene::superSampleRayTraceImpl(float row, float col) {
    if (m_color_map.find(std::make_pair(row, col)) != m_color_map.end())
        return m_color_map[std::make_pair(row, col)];

    glm::vec4 color = rayTrace(row, col);
    m_color_map.emplace(std::make_pair(row, col), color);
    return color;
}

glm::vec4 RayScene::calcLight(const Ray& ray, const Intersect& intersect){
    CS123TransformPrimitive * transprim = intersect.transprim;

    glm::vec4 final (0), ambient, diffuse, specular, lightColor;
    glm::vec4 vertexToLight, vertexToEye, lightReflect;

    float shininess         = transprim->primitive.material.shininess,
          dist              = 0.0f,
          attenuation       = 1.0f,
          diffuseIntensity  = 0.0f,
          specularIntensity = 0.0f;


    ambient  = transprim->primitive.material.cAmbient;
    diffuse  = getDiffuse(intersect);
    specular = transprim->primitive.material.cSpecular;

    final += ambient;

    vertexToEye = glm::normalize(ray.start - intersect.pos_worldSpace);

    for (CS123SceneLightData& light : m_lights) {
        switch (light.type) {
            case LightType::LIGHT_POINT:
                if (!settings.usePointLights)
                    continue;
                vertexToLight = glm::normalize(light.pos - intersect.pos_worldSpace);
                dist = glm::length(vertexToLight);
                attenuation = glm::min(1.0f, 1.0f / (light.function.x + light.function.y * dist + light.function.z * dist * dist));
                break;
            case LightType::LIGHT_DIRECTIONAL:
                if (!settings.useDirectionalLights)
                    continue;
                vertexToLight = glm::normalize(glm::vec4(-glm::vec3(light.dir), 0));
                attenuation = 1.0f;
                break;
            case LightType::LIGHT_SPOT:
                perror("Unsupported light type.");
                continue;
                break;
            case LightType::LIGHT_AREA:
                perror("Unsupported light type.");
                continue;
                break;
        }

        if (settings.useShadows) {
            Ray ray_to_light;
            ray_to_light.delta = vertexToLight;
            ray_to_light.start = intersect.pos_worldSpace + RAY_OFFSET * ray_to_light.delta;
            // Light ray might hit light before hitting an object
            float light_t = glm::length(light.pos - intersect.pos_worldSpace) / glm::length(ray_to_light.delta);

            Intersect ray_light_intersect;
            if (settings.useKDTree)
                kdTreeIntersect(&m_kd_root, ray_to_light, ray_light_intersect);
            else
                naiveIntersect(ray_to_light, ray_light_intersect);

            if (!ray_light_intersect.miss && ray_light_intersect.t < light_t)
                continue;
        }

        lightColor       = light.color;

        diffuseIntensity = glm::max(0.f, glm::dot(vertexToLight, intersect.norm_worldSpace));

        lightReflect      = glm::normalize(glm::reflect(-vertexToLight, intersect.norm_worldSpace));
        specularIntensity = glm::pow(glm::max(0.0f, glm::dot(vertexToEye, lightReflect)), shininess);

        final += attenuation * lightColor * (diffuse * diffuseIntensity + specular * specularIntensity);
    }

    return final;
} // RayScene::calcLight

bool RayScene::getRefractRay(const Ray& incident_ray, const Intersect& intersect, Ray& refract_ray, float ior) {
    // Use formula 16.33 from the textbook
//    ior = intersect.inside ? 1.0f / ior : ior;
    if (intersect.inside)
        ior = 1.0f / ior;
    float sq = 1.0f - ior * ior * (1.0f - glm::pow(glm::dot(intersect.norm_worldSpace, -incident_ray.delta), 2.f));
    if (sq < 0.f)
        return false;

    float c = ior * glm::dot(intersect.norm_worldSpace, -incident_ray.delta) - glm::sqrt(sq);
    refract_ray.delta = glm::normalize(c * intersect.norm_worldSpace + ior * incident_ray.delta);
    refract_ray.start = intersect.pos_worldSpace + RAY_OFFSET * refract_ray.delta;
    return true;
}

glm::vec4 RayScene::getDiffuse(const Intersect& intersect) {

    CS123TransformPrimitive * transprim = intersect.transprim;

    // No texture
    if (!transprim->primitive.material.textureMap.isUsed ||
         transprim->primitive.material.textureMap.filename.empty() ||
         m_texture_images.find(transprim->primitive.material.textureMap.filename) == m_texture_images.end() ||
         !settings.useTextureMapping)
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
    uv = shape->getUV(intersect.pos_objSpace,
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

BGRA RayScene::vec2bgra(glm::vec4 color) {
    BGRA bgra;
    bgra.r = FilterUtils::REAL2byte(color.r);
    bgra.g = FilterUtils::REAL2byte(color.g);
    bgra.b = FilterUtils::REAL2byte(color.b);
    bgra.a = 255;
    return bgra;
}

float RayScene::colorVariation(glm::vec4 colorA, glm::vec4 colorB) {
    return glm::distance(colorA, colorB);
}
