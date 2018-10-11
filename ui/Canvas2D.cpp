/**
 * @file Canvas2D.cpp
 *
 * CS123 2-dimensional canvas. Contains support code necessary for Brush,
 * Filter, Intersect, and
 * Ray.
 *
 * YOU WILL NEED TO FILL THIS IN!
 *
 */

// For your convenience, a few headers are included for you.
#include <iostream>
#include <math.h>
#include <memory>
#include <unistd.h>
#include "Canvas2D.h"
#include "Settings.h"
#include "RayScene.h"

#include <QPainter>
#include "brush/Brush.h"
#include "brush/ConstantBrush.h"
#include "brush/LinearBrush.h"
#include "brush/QuadraticBrush.h"
#include "brush/SmudgeBrush.h"
#include "brush/SpecialBrush1.h"
#include "brush/SpecialBrush2.h"

#include "filters/filter.h"
#include "filters/filterblur.h"
#include "filters/filteredgedetect.h"
#include "filters/filterrotate.h"
#include "filters/filterscale.h"

Canvas2D::Canvas2D() :

// @TODO: Initialize any pointers in this class here.
    m_rayScene(nullptr), m_brush(nullptr){
    set_brush();
    set_filter();
}

Canvas2D::~Canvas2D()
{ }

// This is called when the canvas size is changed. You can change the canvas
// size by calling
// resize(...). You probably won't need to fill this in, but you can if you want
// to.
void Canvas2D::notifySizeChanged(int w, int h){ }

void Canvas2D::paintEvent(QPaintEvent * e){
    // You probably won't need to fill this in, but you can if you want to
    // override any painting
    // events for the 2D canvas. For now, we simply call the superclass.
    SupportCanvas2D::paintEvent(e);
}

// ********************************************************************************************
// ** BRUSH
// ********************************************************************************************


void Canvas2D::mouseDown(int x, int y){
    // @TODO: [BRUSH] Mouse interaction for brush. You will probably want to
    // create a separate
    //        class for each of your brushes. Remember that you can use the
    // static Settings
    //        object to get the currently selected brush and its parameters.

    // You're going to need to leave the alpha value on the canvas itself at
    // 255, but you will
    // need to use the actual alpha value to compute the new color of the pixel

    bool fixAlphaBlending = settings.fixAlphaBlending; // for extra/half credit

    if ((x < 0) || (y < 0)) return;

    m_brush->createNewDrawingLayer(this, fixAlphaBlending);
    m_brush->brushDown(x, y, this);
}

void Canvas2D::mouseDragged(int x, int y){
    // qDebug("Mouse Dragged at %d, %d.", x, y);
    if ((x < 0) || (y < 0)) return;

    m_brush->brushDragged(x, y, this);
}

void Canvas2D::mouseUp(int x, int y){
    // qDebug("Mouse Up at %d, %d.", x, y);
    m_brush->brushUp(x, y, this);
}

// ********************************************************************************************
// ** FILTER
// ********************************************************************************************

void Canvas2D::filterImage(){
    // TODO: [FILTER] Filter the image. Some example code to get the filter type
    // is provided below.

    m_filter->apply(this);

    this->update();
}

void Canvas2D::setScene(RayScene * scene){
    m_rayScene.reset(scene);
}

void Canvas2D::renderImage(Camera * camera, int width, int height){
    if (m_rayScene) {
        // @TODO: raytrace the scene based on settings
        //        YOU MUST FILL THIS IN FOR INTERSECT/RAY

        // If you want the interface to stay responsive, make sure to call
        // QCoreApplication::processEvents() periodically during the rendering.
    }
}

void Canvas2D::cancelRender(){
    // TODO: cancel the raytracer (optional)
}

void Canvas2D::settingsChanged(){
    set_brush();
    set_filter();
}

void Canvas2D::set_brush(){
    BGRA bgra = settings.brushColor;
    int rad   = settings.brushRadius;

    switch (settings.brushType) {
        case BRUSH_CONSTANT:
            m_brush = std::make_unique<ConstantBrush>(bgra, rad);
            break;

        case BRUSH_LINEAR:
            m_brush = std::make_unique<LinearBrush>(bgra, rad);
            break;

        case BRUSH_QUADRATIC:
            m_brush = std::make_unique<QuadraticBrush>(bgra, rad);
            break;

        case BRUSH_SMUDGE:
            m_brush = std::make_unique<SmudgeBrush>(bgra, rad);
            break;

        case BRUSH_SPECIAL_1:
            m_brush = std::make_unique<SpecialBrush1>(bgra, rad);
            break;

        case BRUSH_SPECIAL_2:
            m_brush = std::make_unique<SpecialBrush2>(bgra, rad);
            break;

        default:
            qErrnoWarning("Invalid brush type!");
            exit(-1);
    }
} // Canvas2D::set_brush

void Canvas2D::set_filter(){
    switch (settings.filterType) {
        case FILTER_BLUR:
            m_filter =
              std::make_unique<FilterBlur>(settings.blurRadius,
                FastBox);
            break;

        // fill in the rest
        case FILTER_EDGE_DETECT:
            m_filter = std::make_unique<FilterEdgeDetect>(
                settings.edgeDetectSensitivity);
            break;

        case FILTER_SCALE:
            m_filter =
              std::make_unique<FilterScale>(settings.scaleX, settings.scaleY);
            break;

        case FILTER_ROTATE:
            m_filter = std::make_unique<FilterRotate>(settings.rotateAngle);
            break;

        case FILTER_SPECIAL_1:
            break;

        case FILTER_SPECIAL_2:
            break;

        case FILTER_SPECIAL_3:
            break;

        default:
            break;
    }
} // Canvas2D::set_filter

void Canvas2D::getMarqueeCanvas2D(MarqueeCanvas2D * mcanv){
    int w = width(), h = height();
    QPoint start = marqueeStart(), stop = marqueeStop();

    if (start != stop) {
        int minX = MIN(start.x(), stop.x()),
          minY   = MIN(start.y(), stop.y());
        int maxX = MAX(start.x(), stop.x()),
          maxY   = MAX(start.y(), stop.y());

        // Detect illegal selections
        if (maxX < 0 || maxY < 0 || minX >= w || minY >= h) {
            mcanv->copyFrom(data(), 0, 0, w, h, w); // Copy entire canvas
            return;
        }

        minX = MAX(0, minX);
        minY = MAX(0, minY);
        maxX = MIN(w - 1, maxX);
        maxY = MIN(h - 1, maxY);

        mcanv->copyFrom(data(), minX, minY, maxX - minX, maxY - minY, w);
    } else {
        mcanv->copyFrom(data(), 0, 0, w, h, w);
    }
}
