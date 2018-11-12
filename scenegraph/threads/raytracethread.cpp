#include "raytracethread.h"
#include "BGRA.h"
#include <random>

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
}

RayTraceMaster::RayTraceMaster(QObject *parent)
    : QThread(parent) {}

RayTraceMaster::RayTraceMaster(RayScene *rayscene, int width, int height, BGRA *data)
    : QThread(0), m_rayscene(rayscene), m_width(width), m_height(height), m_data(data), m_subSize(30)
{
    m_row = 0;
    m_col = 0;

    int nTrunkH = std::ceil(static_cast<float>(width) / m_subSize);
    int nTrunkV = std::ceil(static_cast<float>(height) / m_subSize);

    for (int i = 0; i < nTrunkV; i++) {
        for (int j = 0; j < nTrunkH; j++) {
            RayTraceThread* thr = new RayTraceThread(rayscene, i * m_subSize, j * m_subSize, m_subSize, width, height, data);
            m_thrds.push_back(thr);
        }
    }

}

RayTraceMaster::~RayTraceMaster() {
}

void RayTraceMaster::run() {
    for (int i = 0; i < m_thrds.size(); i++) {
        m_thrds[i]->start();
    }
}
