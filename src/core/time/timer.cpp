#include "timer.h"
#include <algorithm>

Timer::Timer()
{
    QueryPerformanceFrequency(&m_freq);
    QueryPerformanceCounter(&m_last);
}

void Timer::Tick()
{
    LARGE_INTEGER curr;
    QueryPerformanceCounter(&curr);

    // 计算真实帧时间
    m_delta = (double)(curr.QuadPart - m_last.QuadPart) / m_freq.QuadPart;

    m_last = curr;
}