#include "GLmain.h"
#include "init_dll.h"

int main() {
	// 初始化DLL目录工具
	if (!DllUtils::InitAddDllDir()) {
		return -1;
	}
	// 添加dll目录
	if (!DllUtils::AddDllDir(L"ku3f\\lib")) {
		return -1;
	}

	static bool wcoutInited = false;
	if (!wcoutInited) {
		std::ios::sync_with_stdio(false); // 解除C/C++流同步（关键）
		std::wcout.imbue(std::locale("")); // 适配系统本地编码（中文/宽字符）
		std::wcout.tie(nullptr); // 解除wcout与wcin的绑定
		wcoutInited = true;
	}

	GLmain();
	return 0;
}
/*
待办事项：
环境光遮蔽 []
天空盒 []
区块内存压缩优化 []
动态加载区块 [*]
绘制提交列表优化 []
半透明模型绘制 []

*/