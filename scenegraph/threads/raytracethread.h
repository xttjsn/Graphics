#ifndef RAYTRACETHREAD_H
#define RAYTRACETHREAD_H

#include <QRunnable>
#include "RayScene.h"

class RayScene;

class RayTraceRunnable : public QRunnable
{
public:
    RayTraceRunnable(RayScene* rayscene, int row, int col, int width, int height, BGRA* data);

protected:
    void run();

private:
    RayScene *m_rayscene;
    int m_row, m_col, m_width, m_height;
    BGRA* m_data;
};

#endif // RAYTRACETHREAD_H
