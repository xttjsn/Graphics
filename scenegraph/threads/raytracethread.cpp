#include "raytracethread.h"
#include "BGRA.h"
#include <math.h>

RayTraceThread::RayTraceThread(QObject* parent)
    : QThread(parent)
{}

RayTraceThread::RayTraceThread(RayScene* rayscene, int row, int col, int sz, int width, int height, BGRA* data)
    : QThread(0), m_rayscene(rayscene), m_row(row), m_col(col), m_sz(sz), m_width(width), m_height(height), m_data(data)
{
}

RayTraceThread::~RayTraceThread() {}

void RayTraceThread::run() {
    BGRA bgra;

    for (int row = m_row; row < std::min(m_height, m_row + m_sz); row++)  {
        for (int col = m_col; col < std::min(m_width, m_col + m_sz); col++) {
            m_rayscene->rayTrace(row, col, m_width, m_height, bgra);
            *(m_data + row * m_width + col) = bgra;
            bgra = BGRA(0,0,0,255);
        }
    }

    emit rayTraceFinish();
}

RayTraceMaster::RayTraceMaster(QObject *parent)
    : QThread(parent) {}

RayTraceMaster::RayTraceMaster(RayScene *rayscene, int width, int height, BGRA *data)
    : QThread(0), m_rayscene(rayscene), m_width(width), m_height(height), m_data(data), m_subSize(30),
      m_nThrds(0), m_nextThr(0), m_nActiveThrds(0), m_nMaxThrds(10)
{
    m_row = 0;
    m_col = 0;

    int nTrunkH = std::ceil(static_cast<float>(width) / m_subSize);
    int nTrunkV = std::ceil(static_cast<float>(height) / m_subSize);

    m_nThrds = nTrunkH * nTrunkV;
    for (int i = 0; i < nTrunkV; i++) {
        for (int j = 0; j < nTrunkH; j++) {
            RayTraceThread* thr = new RayTraceThread(rayscene, i * m_subSize, j * m_subSize, m_subSize, width, height, data);
            QObject::connect(thr, SIGNAL(rayTraceFinish()), this, SLOT(ThreadFinished()));
            m_thrds.push_back(thr);
        }
    }
}

RayTraceMaster::~RayTraceMaster() {
}

void RayTraceMaster::run() {
    m_nextThr = 0;
    m_nActiveThrds = 0;
    for (int i = 0; i < std::min((int)m_thrds.size(), m_nMaxThrds); i++, m_nActiveThrds++) {
        m_thrds[m_nextThr++]->start();
    }
}

void RayTraceMaster::ThreadFinished() {
    m_nActiveThrds--;
    while (m_nActiveThrds < m_nMaxThrds && m_nextThr < m_nThrds) {
        m_thrds[m_nextThr++]->start();
        m_nActiveThrds++;
    }
}
