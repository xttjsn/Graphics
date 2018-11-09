#include "RayScene.h"
#include "Settings.h"
#include "CS123SceneData.h"

#include <algorithm>
#include <iostream>

RayScene::RayScene(Scene &scene) :
    Scene(scene)
{
    // TODO [INTERSECT]
    // Remember that any pointers or OpenGL objects (e.g. texture IDs) will
    // be deleted when the old scene is deleted (assuming you are managing
    // all your memory properly to prevent memory leaks).  As a result, you
    // may need to re-allocate some things here.
    loadTextures();
    loadKDTree();
}

RayScene::~RayScene()
{
}

void RayScene::loadTextures() {
    for (CS123TransformPrimitive &transPrim : m_transPrims) {
        loadMapData(transPrim.primitive.material);
    }
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

    // Step 1 - compute boudning boxes and surface areas for all objects and
    //     put them in three vectors in sorted order.
    float surface, xMin = FLT_MAX, xMax = FLT_MIN, yMin = FLT_MAX, yMax = FLT_MIN, zMin = FLT_MAX, zMax = FLT_MIN;
    BoundingBox bbox;
    ImplicitShape* shape;
    KDTreePrimitive kdprim;
    for (CS123TransformPrimitive& transprim : m_transPrims) {
        shape = getShapePointer(transprim.primitive.type);
        shape->setTransform(transprim.transform);
        surface = shape->surfaceArea();
        bbox = shape->boundingBox();
        xMin = std::min(xMin, bbox.xMin);
        xMax = std::max(xMax, bbox.xMax);
        yMin = std::min(yMin, bbox.yMin);
        yMax = std::max(yMax, bbox.yMax);
        zMin = std::min(zMin, bbox.zMin);
        zMax = std::max(zMax, bbox.zMax);

        kdprim.bbox = bbox;
        kdprim.surface = surface;
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

    root->left = new KDTreeNode();
    root->right = new KDTreeNode();

    if (costX < costY && costX < costZ) {
        // Split along X-axis
        root->left->surface = surfaceLX;
        root->right->surface = surfaceRX;
        root->left->bbox = BoundingBox(root->bbox.xMin, splitX,
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
        root->left->surface = surfaceLY;
        root->right->surface = surfaceRY;
        root->left->bbox = BoundingBox(root->bbox.xMin, root->bbox.xMax,
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
        root->left->surface = surfaceLZ;
        root->right->surface = surfaceRZ;
        root->left->bbox = BoundingBox(root->bbox.xMin, root->bbox.xMax,
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

    std::sort(root->primitives.begin(), root->primitives.end(), axis == AXIS_X ? less_than_x_key() :
                                                                                 (axis == AXIS_Y ? less_than_y_key() :
                                                                                                   less_than_z_key()));
    for (int i = 0; i < root->primitives.size() - 1; i++) {

        surfaceLeftBase += root->primitives[i].surface;
        surfaceRightBase -= root->primitives[i].surface;

        surfaceLeft = surfaceLeftBase;
        countLeft++;

        // Handle intersected objects
        for (int j = i + 1; j < root->primitives.size() - 1; j++) {
            bool within = axis == AXIS_X ? root->primitives[j].bbox.xMin <= root->primitives[i].xMax :
                                           (axis == AXIS_Y ? root->primitives[j].bbox.yMin <= root->primitives[i].yMax :
                                                             root->primitives[j].bbox.zMin <= root->primitives[i].zMax);
            if (within) {
                surfaceLeft += root->primitives[j].surface;
                countLeft++;
            } else
                break;
        }

        surfaceRight = surfaceRightBase;
        countRight--;

        weightLeft = surfaceLeft / root->surface;
        weightRight = surfaceRight / root->surface;
        cost = weightLeft * countLeft + weightRight * countRight;

        if (cost < mincost) {
            mincost = cost;
            split = axis == AXIS_X ? root->primitives[i].bbox.xMax :
                                     (axis == AXIS_Y ? root->primitives[i].bbox.yMax :
                                                       root->primitives[i].bbox.zMax);
            surfaceL = surfaceLeft;
            surfaceR = surfaceRight;
        }
    }

}

void RayScene::render(SupportCanvas2D* canvas, Camera* camera, int width, int height) {
    BGRA* data = canvas->data();

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            BGRA bgra;
            rayTrace(camera, r, c, width, height, bgra);
            *(data + r * width + w) = bgra;
        }
    }
}

void RayScene::rayTrace(Camera* camera, int row, int col, int width, int height, BGRA& bgra) {
    Ray ray;

    // Get camera related matrices
    glm::mat4x4 view     = camera->getViewMatrix();
    glm::mat4x4 scale    = camera->getScaleMatrix();
    glm::mat4x4 viewInv  = glm::inverse(view);
    glm::mat4x4 scaleInv = glm::inverse(scale);

    // Generate ray in camera space, and then transform it into world space
    glm::vec4 filmPixelPos = getFilmPixelPosition(row, col, width, height);
    filmPixelPos = viewInv * scaleInv * filmPixelPos;
    ray.start    = glm::vec4(0, 0, 0, 1);
    ray.start    = viewInv * ray.start;
    ray.delta    = glm::normalize(filmPixelPos - ray.start);

    // Try to find an intersection using acceleration data structure
    Intersect intersect;
    kdTreeIntersect(&m_kd_root, ray, intersect);

    if (intersect.miss) {
        return;
    }

    CS123TransformPrimitive* transprim = intersect.transprim;
    ImplicitShape* shape = getPrimitivePointer(transprim->primitive.type);
    shape->setTransform(transprim->transform);

    // Compute illumination
    glm::vec4 final, ambient, diffuse;
    ambient = transprim->primitive.material.cAmbient;
    diffuse = transprim->primitive.material.cDiffuse;

    for (CS123SceneLightData& light : m_lights) {
        switch (light.type) {
        case LIGHT_POINT:
            glm::vec4 pos        = light.pos;
            glm::vec4 lightColor = light.color;
            glm::vec4 norm       = shape->normal(intersect);
            float cos            = glm::dot(glm::normalize(norm), glm::normalize(pos - intersect.pos));
            final += lightColor * diffuse * cos;
            break;
        case LIGHT_DIRECTIONAL:
            glm::vec4 dir        = light.dir;
            glm::vec4 lightColor = light.color;
            glm::vec4 norm       = shape->normal(intersect);
            float cos            = glm::dot(glm::normalize(norm), glm::normalize(-dir));
            final += lightColor * diffuse * cos;
            break;
        case LIGHT_SPOT:
            perror("Unsupported light type.");
            exit(1);
            break;
        case LIGHT_AREA:
            perror("Unsupported light type.");
            exit(1);
            break;
        }
    }

    final += ambient;

    bgra.r = glm::clamp(final.r, 0, 1) * 255;
    bgra.g = glm::clamp(final.g, 0, 1) * 255;
    bgra.b = glm::clamp(final.b, 0, 1) * 255;
    bgra.a = 255;

    returnShapePointer(transprim->primitive.type, shape);
}

ImplicitShape* RayScene::getShapePointer(PrimitiveType type) {
    ImplicitShape* shape;
    switch (type) {
    case PRIMITIVE_CUBE:
        shape = m_cube.release();
        break;

    case PRIMITIVE_CONE:
        shape = m_cone.release();
        break;

    case PRIMITIVE_CYLINDER:
        shape = m_cylinder.release();
        break;

    case PRIMITIVE_TORUS:
        shape = m_torus.release();
        break;

    case PRIMITIVE_SPHERE:
        shape = m_sphere.release();
        break;
    }

    return shape;
}

void RayScene::returnShapePointer(PrimitiveType type, ImplicitShape* shape) {
    switch (type) {
    case PRIMITIVE_CUBE:
        m_cube.reset(static_cast<ImplicitCube*>(shape));
        break;

    case PRIMITIVE_CONE:
        m_cone.reset(static_cast<ImplicitCone*>(shape));
        break;

    case PRIMITIVE_CYLINDER:
        m_cylinder.reset(static_cast<ImplicitCylinder*>(shape));
        break;

    case PRIMITIVE_TORUS:
        m_torus.reset(static_cast<ImplicitTorus*>(shape));
        break;

    case PRIMITIVE_SPHERE:
        m_sphere.reset(static_cast<ImplicitSphere*>(shape));
        break;
    }
}

glm::vec4 RayScene::getFilmPixelPosition(int row, int col, int width, int height) {
    return glm::vec4(static_cast<float>(2 * col - width) / static_cast<float>(width),
                     static_cast<float>(height - 2 * row)/ static_cast<float>(height),
                     -1,
                     1);
}

void RayScene::kdTreeIntersect(KDTreeNode* root, Ray& ray, Intersect& intersect) {
    // Treat each bounding box as an ImplicitCube, compute the intersectons with both of them, and take the smaller one (if any)
    ImplicitCube cubeLeft, cubeRight;
    KDTreeNode *left = root.left, *right = root.right;

    if (left && right)  {
        // If the current root is not a leaf node
        glm::mat4x4 leftTransform  = boundingBoxToTransform(left->bbox);
        glm::mat4x4 rightTransform = boundingBoxToTransform(right->bbox);

        cubeLeft.setTransform(leftTransform);
        cubeRight.setTransform(rightTransform);

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
            shape = getShapePointer(transprim->primitive.type);
            shape->setTransform(transprim->transform);
            itsct = shape->intersect(ray);
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
