#include "setFPS.h"
SetFPS::SetFPS(double* deltatime) {
	this->deltatime = deltatime;
}


void SetFPS::setFPS(double fps){
	this->fps = fps;
}

// 高精度忙等待函数
void SetFPS::BusyWait(double microseconds) {
	LARGE_INTEGER frequency, start, end;
	double interval;

	// 获取性能计数器的频率
	QueryPerformanceFrequency(&frequency);

	// 获取当前时间
	QueryPerformanceCounter(&start);

	// 计算需要等待的时间（以计数器频率为单位）
	interval = (double)microseconds * frequency.QuadPart / 1000000.0;

	// 计算目标时间
	end.QuadPart = start.QuadPart + (LONGLONG)interval;

	// 忙等待循环，直到当前时间超过目标时间
	while (true) {
		QueryPerformanceCounter(&start);
		if (start.QuadPart >= end.QuadPart) {
			break;
		}
	}
}
//设置FPS
void SetFPS::update() {
	static LARGE_INTEGER frequency;
	static LARGE_INTEGER lastTime; // 添加lastTime定义
	static bool initialized = false;

	if (!initialized) {
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&lastTime); // 初始化lastTime
		initialized = true;
	}

	double fpstime = 1.0 / fps;
	LARGE_INTEGER current;

	// 获取当前时间
	QueryPerformanceCounter(&current);

	// 计算距离下一帧开始还需要等待的时间
	double delta = (double)(current.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
	if (delta < fpstime) {
		BusyWait((fpstime - delta) * 1000000.0);
		QueryPerformanceCounter(&current);
		delta = (double)(current.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
	}
	*deltatime = (double)(current.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
	// 更新deltatime和lastTime

	lastTime = current;
	//std::cout << "FPS: " << 1.0 / (*deltatime) << "\n";
}