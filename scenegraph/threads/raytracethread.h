#ifndef RAYTRACETHREAD_H
#define RAYTRACETHREAD_H

class RayScene;
class RayTraceThread;
class RayTraceMaster;

#include <QThread>
#include <QObject>
#include "RayScene.h"

class RayTraceMaster : public QThread
{
    Q_OBJECT;

public:
    RayTraceMaster(QObject *parent = 0);
    RayTraceMaster(RayScene* rayscene, SupportCanvas2D* canvas);
    ~RayTraceMaster();

public slots:
    void ThreadFinished();

protected:
    void run();

private:

    RayScene *m_rayscene;
    int m_row, m_col, m_width, m_height, m_subSize;
    int m_nThrds, m_nextThr, m_nActiveThrds, m_nMaxThrds;
    BGRA* m_data;
    std::vector<RayTraceThread*> m_thrds;
    SupportCanvas2D *m_canvas;
};
class RayTraceThread : public QThread
{
    Q_OBJECT

public:
    RayTraceThread(QObject *parent = 0);
    RayTraceThread(RayScene* rayscene, SupportCanvas2D* canvas, int row, int col, int sz);
    ~RayTraceThread();

signals:
    void rayTraceFinish();

protected:
    void run();

private:
    float bgraDifference(const BGRA& A, const BGRA& B, const BGRA& C, const BGRA& D);
    void averageBGRA(const BGRA& A, const BGRA& B, const BGRA& C, const BGRA& D, BGRA& res);

    RayScene *m_rayscene;
    int m_row, m_col, m_sz, m_width, m_height;
    BGRA* m_data;
    SupportCanvas2D* m_canvas;
};

#endif // RAYTRACETHREAD_H
