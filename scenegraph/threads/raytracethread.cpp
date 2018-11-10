#include "raytracethread.h"
#include "BGRA.h"

RayTraceRunnable::RayTraceRunnable(RayScene* rayscene, int row, int col, int width, int height, BGRA* data)
    : m_rayscene(rayscene), m_row(row), m_col(col), m_width(width), m_height(height), m_data(data)
{
}

void RayTraceRunnable::run() {
    BGRA bgra;
    m_rayscene->rayTrace(m_row, m_col, m_width, m_height, bgra);
    *(m_data + m_row * m_width + m_col) = bgra;
}
