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

    float change, maxChange = 1.5;
    BGRA LU, RU, LD, RD;
    for (int row = m_row; row < std::min(m_height, m_row + m_sz) - 1; row++)  {
        for (int col = m_col; col < std::min(m_width, m_col + m_sz) - 1; col++) {
            LU = *(m_data + row * m_width + col);
            RU = *(m_data + row * m_width + col + 1);
            LD = *(m_data + (row + 1) * m_width + col);
            RD = *(m_data + (row + 1) * m_width + col + 1);
            change = bgraDifference(LU, RU, LD, RD);

//            printf("bgraDifference = %f\n", change);
            float step = 0.5;
            if (change > maxChange) {
                m_rayscene->rayTrace(row - step, col - step, m_width, m_height, LU);
                m_rayscene->rayTrace(row - step, col + step, m_width, m_height, RU);
                m_rayscene->rayTrace(row + step, col - step, m_width, m_height, LD);
                m_rayscene->rayTrace(row + step, col + step, m_width, m_height, RD);
                averageBGRA(LU, RU, LD, RD, bgra);
                *(m_data + row * m_width + col) = bgra;
            }
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
    while (m_nActiveThrds  < m_nMaxThrds && m_nextThr < m_nThrds) {
        m_thrds[m_nextThr++]->start();
        m_nActiveThrds++;
    }
}

float RayTraceThread::bgraDifference(const BGRA& A, const BGRA& B, const BGRA& C, const BGRA& D) {
    float diff = 0;
    diff += std::fabs(A.r - B.r) + std::fabs(A.r - C.r) + std::fabs(A.r - D.r) +
            std::fabs(B.r - C.r) + std::fabs(B.r - D.r) +
            std::fabs(C.r - D.r) +
            std::fabs(A.g - B.g) + std::fabs(A.g - C.g) + std::fabs(A.g - D.g) +
            std::fabs(B.g - C.g) + std::fabs(B.g - D.g) +
            std::fabs(C.g - D.g) +
            std::fabs(A.b - B.b) + std::fabs(A.b - C.b) + std::fabs(A.b - D.b) +
            std::fabs(B.b - C.b) + std::fabs(B.b - D.b) +
            std::fabs(C.b - D.b);
    diff /= 18.f;
    return diff;
}

void RayTraceThread::averageBGRA(const BGRA& A, const BGRA& B, const BGRA& C, const BGRA& D, BGRA& res) {
    res.r = std::round((static_cast<int>(A.r) + static_cast<int>(B.r) + static_cast<int>(C.r) + static_cast<int>(D.r)) / 4.f);
    res.g = std::round((static_cast<int>(A.g) + static_cast<int>(B.g) + static_cast<int>(C.g) + static_cast<int>(D.g)) / 4.f);
    res.b = std::round((static_cast<int>(A.b) + static_cast<int>(B.b) + static_cast<int>(C.b) + static_cast<int>(D.b)) / 4.f);
    res.a = 255;
}
