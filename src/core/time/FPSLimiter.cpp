#include "FPSLimiter.h"
#pragma comment(lib, "winmm.lib") 
FPSLimiter::FPSLimiter(double maxFPS)
{
    QueryPerformanceFrequency(&m_freq);
    SetMaxFPS(maxFPS);
}

void FPSLimiter::SetMaxFPS(double maxFPS)
{
    m_maxFrameTime = (maxFPS > 0.0) ? 1.0 / maxFPS : 0.0;
}

void FPSLimiter::Wait()
{
    // 不限制帧率直接返回
    if (m_maxFrameTime <= 0.0)
    {
        QueryPerformanceCounter(&m_frameStart);
        return;
    }

    LARGE_INTEGER curr;
    QueryPerformanceCounter(&curr);

    // 已用时间（秒）
    const double elapsed = static_cast<double>(curr.QuadPart - m_frameStart.QuadPart)
        / static_cast<double>(m_freq.QuadPart);

    // 剩余需要等待的时间
    double remain = m_maxFrameTime - elapsed;

    // Windows 提高定时器精度（必须！否则帧率不准）
    timeBeginPeriod(1);

    // 等待循环
    while (remain > 0.0)
    {
        if (remain > 0.002)  // 剩余 > 2ms：用 Sleep 降低 CPU
        {
            // 休眠 (毫秒)，减 1ms 预留补偿精度误差
            Sleep(static_cast<DWORD>(remain * 1000 - 1));
        }
        else  // 最后 < 2ms：轻量自旋等待（高精度）
        {
            // 极短忙等，Windows 下最稳
            Sleep(0);
        }

        // 更新剩余时间
        QueryPerformanceCounter(&curr);
        remain = m_maxFrameTime - static_cast<double>(curr.QuadPart - m_frameStart.QuadPart)
            / static_cast<double>(m_freq.QuadPart);
    }

    // 恢复定时器精度
    timeEndPeriod(1);

    // 记录下一帧开始时间
    QueryPerformanceCounter(&m_frameStart);
}