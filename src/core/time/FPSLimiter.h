#pragma once
#include <windows.h>

class FPSLimiter
{
public:
    explicit FPSLimiter(double maxFPS = 144.0);
    void SetMaxFPS(double maxFPS);
    void Wait();

private:
    double m_maxFrameTime;
    LARGE_INTEGER m_freq;
    LARGE_INTEGER m_frameStart;
};