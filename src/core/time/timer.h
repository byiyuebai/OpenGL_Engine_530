#pragma once
#include <windows.h>

class Timer
{
public:
    Timer();
    void Tick();    // 每帧调用一次
    double DeltaTime() const { return m_delta; }

private:
    LARGE_INTEGER m_freq;
    LARGE_INTEGER m_last;
    double m_delta = 0.0;
};