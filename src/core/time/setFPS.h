#pragma once
#include <windows.h>
#include <iostream>

class SetFPS {
public:
	SetFPS(double* deltatime);

	void setFPS(double fps);
	void update();
	void BusyWait(double microseconds);

private:
	//一个deltatime指针,指向game类中定义的deltatime变量
	double* deltatime;

	double fps=60;
};