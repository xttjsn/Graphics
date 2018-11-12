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
    RayTraceMaster(RayScene* rayscene, int width, int height, BGRA* data);
    ~RayTraceMaster();

public slots:
    void ThreadFinished();

protected:
    void run();

    RayScene *m_rayscene;
    int m_row, m_col, m_width, m_height, m_subSize;
    int m_nThrds, m_nextThr, m_nActiveThrds, m_nMaxThrds;
    BGRA* m_data;
    std::vector<RayTraceThread*> m_thrds;
};
class RayTraceThread : public QThread
{
    Q_OBJECT

public:
    RayTraceThread(QObject *parent = 0);
    RayTraceThread(RayScene* rayscene, int row, int col, int sz, int width, int height, BGRA* data);
    ~RayTraceThread();

signals:
    void rayTraceFinish();

protected:
    void run();

private:
    RayScene *m_rayscene;
    int m_row, m_col, m_sz, m_width, m_height;
    BGRA* m_data;
};

#endif // RAYTRACETHREAD_H
