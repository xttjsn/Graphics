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
    Scene(scene)
{
    // TODO [INTERSECT]
    // Remember that any pointers or OpenGL objects (e.g. texture IDs) will
    // be deleted when the old scene is deleted (assuming you are managing
    // all your memory properly to prevent memory leaks).  As a result, you
    // may need to re-allocate some things here.
    loadTextures();
    loadShapes();
//    loadKDTree();
}

RayScene::~RayScene()
{
}

void RayScene::loadTextures() {
    for (CS123TransformPrimitive &transPrim : m_transPrims) {
        loadMapData(transPrim.primitive.material);
    }
}

void RayScene::loadShapes() {
    m_cone     = std::make_unique<ImplicitCone>();
    m_cube     = std::make_unique<ImplicitCube>();
    m_sphere   = std::make_unique<ImplicitSphere>();
    m_cylinder = std::make_unique<ImplicitCylinder>();
    m_torus    = std::make_unique<ImplicitTorus>();
}

void RayScene::loadMapData(CS123SceneMaterial& mat) {
    if (!mat.textureMap.isUsed || mat.textureMap.filename.empty())
        return;

    if (m_textures.find(mat.textureMap.filename) != m_textures.end())
        return;

    QImage img(mat.textureMap.filename.data());
    img = QGLWidget::convertToGLFormat(img);

    if (img.isNull())
        return;

    Texture2D tex(img.bits(), img.width(), img.height());

    TextureParametersBuilder builder;
    builder.setFilter(TextureParameters::FILTER_METHOD::LINEAR);
    builder.setWrap(TextureParameters::WRAP_METHOD::REPEAT);
    TextureParameters params = builder.build();
    params.applyTo(tex);

    m_textures.emplace(mat.textureMap.filename, std::move(tex));
}

void RayScene::loadKDTree() {

    float surface, xMin = FLT_MAX, xMax = FLT_MIN, yMin = FLT_MAX, yMax = FLT_MIN, zMin = FLT_MAX, zMax = FLT_MIN;
    BoundingBox bbox;
    ImplicitShape* shape;
    KDTreePrimitive kdprim;
    for (CS123TransformPrimitive& transprim : m_transPrims) {
        shape = getShapePointer(transprim.primitive.type);
        shape->setTransform(transprim.transform, transprim.transformInv);
        surface = shape->surfaceArea();
        bbox    = shape->boundingBox();
        returnShapePointer(transprim.primitive.type, shape);
        xMin    = std::min(xMin, bbox.xMin);
        xMax    = std::max(xMax, bbox.xMax);
        yMin    = std::min(yMin, bbox.yMin);
        yMax    = std::max(yMax, bbox.yMax);
        zMin    = std::min(zMin, bbox.zMin);
        zMax    = std::max(zMax, bbox.zMax);

        kdprim.bbox      = bbox;
        kdprim.surface   = surface;
        kdprim.transprim = &transprim;
        m_kd_root.primitives.push_back(kdprim);
    }
    m_kd_root.bbox = BoundingBox(xMin, xMax, yMin, yMax, zMin, zMax);

    split(&m_kd_root);
}

void RayScene::split(KDTreeNode* root) {

    float costNotSplit = root->primitives.size(), costX = FLT_MAX, costY = FLT_MAX, costZ = FLT_MAX, splitX, splitY, splitZ;
    float surfaceLX, surfaceRX, surfaceLY, surfaceRY, surfaceLZ, surfaceRZ;

    trySplit(root, costX, splitX, surfaceLX, surfaceRX, AXIS_X);
    trySplit(root, costY, splitY, surfaceLY, surfaceRY, AXIS_Y);
    trySplit(root, costZ, splitZ, surfaceLZ, surfaceRZ, AXIS_Z);

    if (costNotSplit < costX && costNotSplit < costY && costNotSplit < costZ)
        return;

    root->left  = new KDTreeNode();
    root->right = new KDTreeNode();

    if (costX < costY && costX < costZ) {
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
            if (prim.bbox.xMin < splitX)
                root->left->primitives.push_back(prim);
            else
                root->right->primitives.push_back(prim);
        }
    }
    else if (costY < costX && costY < costZ) {
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
            if (prim.bbox.yMin < splitY)
                root->left->primitives.push_back(prim);
            else
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
            if (prim.bbox.zMin < splitZ)
                root->left->primitives.push_back(prim);
            else
                root->right->primitives.push_back(prim);
        }
    }

    // Don't need primitives anymore
    root->primitives.clear();

    split(root->left);
    split(root->right);
}

void RayScene::trySplit(KDTreeNode* root, float& mincost, float& split, float& surfaceL, float& surfaceR, Axis axis) {

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
        for (unsigned int j = i + 1; j < root->primitives.size() - 1; j++) {
            bool within = axis == AXIS_X ? root->primitives[j].bbox.xMin <= root->primitives[i].bbox.xMax :
                          (axis == AXIS_Y ? root->primitives[j].bbox.yMin <= root->primitives[i].bbox.yMax :
                           root->primitives[j].bbox.zMin <= root->primitives[i].bbox.zMax);
            if (within) {
                surfaceLeft += root->primitives[j].surface;
                countLeft++;
            } else
                break;
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

}

void RayScene::render(SupportCanvas2D* canvas, Camera* camera, int width, int height) {
    BGRA* data = canvas->data();
    loadCameraMatrices(camera);
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            RayTraceRunnable* task = new RayTraceRunnable(this, r, c, width, height, data);
            QThreadPool::globalInstance()->start(task);
//            rayTrace(r, c, width, height, bgra);
//            *(data + r * width + c) = bgra;
        }
    }
}

void RayScene::loadCameraMatrices(Camera* camera) {
    m_view = camera->getViewMatrix();
    m_scale = camera->getScaleMatrix();
    m_viewInv = glm::inverse(m_view);
    m_scaleInv = glm::inverse(m_scale);

}

void RayScene::rayTrace(int row, int col, int width, int height, BGRA& bgra) {
    Ray ray;

    // Get camera related matrices
    glm::mat4x4 model;
    glm::mat4x4 modelInv;

    // Generate ray in camera space, and then transform it into world space
    glm::vec4 filmPixelPos = getFilmPixelPosition(row, col, width, height);
    filmPixelPos = m_viewInv * m_scaleInv * filmPixelPos;
    ray.start    = m_viewInv * glm::vec4(0, 0, 0, 1);
    ray.delta    = glm::normalize(filmPixelPos - ray.start);

    // Try to find an intersection using acceleration data structure
    Intersect intersect;
//    kdTreeIntersect(&m_kd_root, ray, intersect);
    naiveIntersect(ray, intersect);

    if (intersect.miss) {
        // DEBUG
        return;
    }


    CS123TransformPrimitive* transprim = intersect.transprim;
    ImplicitShape* shape               = getShapePointer(transprim->primitive.type);
    model                              = transprim->transform;
    modelInv                           = transprim->transformInv;
    shape->setTransform(transprim->transform, transprim->transformInv);

    // Compute illumination
    glm::vec4 final (0), ambient, diffuse;
    ambient = transprim->primitive.material.cAmbient;
    diffuse = transprim->primitive.material.cDiffuse;

    glm::vec4 normal = shape->normal(intersect);
    glm::vec4 position_cameraSpace = intersect.pos;
    glm::vec4 normal_cameraSpace = glm::vec4(glm::normalize(glm::mat3(glm::transpose(modelInv)) * glm::vec3(normal)), 0);

    final = ambient;
    glm::vec4 lightColor;
    glm::vec4 vertexToLight;
    float diffuseIntensity;

    for (CS123SceneLightData& light : m_lights) {
        switch (light.type) {
        case LightType::LIGHT_POINT:
            vertexToLight = glm::normalize(glm::vec4(glm::vec3(light.pos), 1) - position_cameraSpace);
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

        lightColor = light.color;
        diffuseIntensity = glm::max(0.f, glm::dot(vertexToLight, normal_cameraSpace));
        final += glm::max(glm::vec4(0), lightColor * diffuse * diffuseIntensity);
    }

    bgra.r = glm::clamp(final.r, 0.f, 1.f) * 255;
    bgra.g = glm::clamp(final.g, 0.f, 1.f) * 255;
    bgra.b = glm::clamp(final.b, 0.f, 1.f) * 255;
    bgra.a = 255;

    returnShapePointer(transprim->primitive.type, shape);
}

ImplicitShape* RayScene::getShapePointer(PrimitiveType type) {
    ImplicitShape* shape;
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
}

void RayScene::returnShapePointer(PrimitiveType type, ImplicitShape* shape) {
    switch (type) {
    case PrimitiveType::PRIMITIVE_CUBE:
        m_cube.reset(static_cast<ImplicitCube*>(shape));
        m_cubemtx.unlock();
        break;

    case PrimitiveType::PRIMITIVE_CONE:
        m_cone.reset(static_cast<ImplicitCone*>(shape));
        m_conemtx.unlock();
        break;

    case PrimitiveType::PRIMITIVE_CYLINDER:
        m_cylinder.reset(static_cast<ImplicitCylinder*>(shape));
        m_cylindermtx.unlock();
        break;

    case PrimitiveType::PRIMITIVE_TORUS:
        m_torus.reset(static_cast<ImplicitTorus*>(shape));
        m_torusmtx.unlock();
        break;

    case PrimitiveType::PRIMITIVE_SPHERE:
        m_sphere.reset(static_cast<ImplicitSphere*>(shape));
        m_spheremtx.unlock();
        break;

    default:
        perror("Unsupported shape.");
        exit(1);
    }
}

glm::vec4 RayScene::getFilmPixelPosition(int row, int col, int width, int height) {
    return glm::vec4(static_cast<float>(2 * col - width) / static_cast<float>(width),
                     static_cast<float>(height - 2 * row)/ static_cast<float>(height),
                     -1,
                     1);
}

void RayScene::naiveIntersect(Ray& ray, Intersect& intersect) {
    ImplicitShape* shape;
    Intersect itsct;
    std::vector<Intersect> itscts;

    for (CS123TransformPrimitive& transprim : m_transPrims) {
        shape = getShapePointer(transprim.primitive.type);
        shape->setTransform(transprim.transform, transprim.transformInv);
        itsct = shape->intersect(ray);
        if (!itsct.miss) {
            itsct.transprim = &transprim;
            itscts.push_back(itsct);
        }
        returnShapePointer(transprim.primitive.type, shape);
    }

    if (itscts.empty())
        intersect = Intersect();
    else
        intersect = *std::min_element(itscts.begin(), itscts.end(), itsctComp());
}

void RayScene::kdTreeIntersect(KDTreeNode* root, Ray& ray, Intersect& intersect) {
    // Treat each bounding box as an ImplicitCube, compute the intersectons with both of them, and take the smaller one (if any)
    ImplicitCube cubeLeft, cubeRight;
    KDTreeNode *left = root->left, *right = root->right;

    if (left && right)  {
        // If the current root is not a leaf node
        glm::mat4x4 leftTransform = left->bbox.transform;
        glm::mat4x4 leftTransformInv = left->bbox.transformInv;
        glm::mat4x4 rightTransform = right->bbox.transform;
        glm::mat4x4 rightTransformInv = right->bbox.transformInv;

        cubeLeft.setTransform(leftTransform, leftTransformInv);
        cubeRight.setTransform(rightTransform, rightTransformInv);

        Intersect lIntersect = cubeLeft.intersect(ray);
        Intersect rIntersect = cubeRight.intersect(ray);

        if (lIntersect.miss && rIntersect.miss) {
            // Miss both children
            return;
        }
        else if (lIntersect.miss) {
            // Only intersect with right children's bounding box
            kdTreeIntersect(right, ray, intersect);
        }
        else if (rIntersect.miss) {
            // Only intersect with left children's bounding box
            kdTreeIntersect(left, ray, intersect);
        } else {
            // Intersect with both children
            if (lIntersect.t < rIntersect.t) {
                kdTreeIntersect(left, ray, intersect);
                if (intersect.miss)
                    kdTreeIntersect(right, ray, intersect);

            } else {
                kdTreeIntersect(right, ray, intersect);
                if (intersect.miss)
                    kdTreeIntersect(left, ray, intersect);
            }
        }
    } else {
        // If it is a leaf node
        Intersect itsct;
        ImplicitShape* shape;
        CS123TransformPrimitive* transprim;
        std::vector<Intersect> itscts;
        for (KDTreePrimitive& kdprim : root->primitives) {
            transprim = kdprim.transprim;
            shape     = getShapePointer(transprim->primitive.type);
            shape->setTransform(transprim->transform, transprim->transformInv);
            itsct           = shape->intersect(ray);
            itsct.transprim = transprim;
            returnShapePointer(transprim->primitive.type, shape);

            if (!itsct.miss) {
                itscts.push_back(itsct);
            }
        }

        // Early stop
        if (!itscts.empty()) {
            intersect = *std::min_element(itscts.begin(), itscts.end(), itsctComp());
        }
    }
}

glm::mat4x4 RayScene::boundingBoxToTransform(BoundingBox& bbox) {
    float xc = (bbox.xMax + bbox.xMin) / 2, yc = (bbox.yMax + bbox.yMin) / 2, zc = (bbox.zMax + bbox.zMin) / 2;
    float xs = bbox.xMax - bbox.xMin, ys = bbox.yMax - bbox.yMin, zs = bbox.zMax - bbox.zMin;
    glm::mat4x4 mat = glm::scale(glm::vec3(xs, ys, zs)) * glm::translate(glm::vec3(xc, yc, zc));
    return mat;
}
