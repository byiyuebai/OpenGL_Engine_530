#pragma once
#include <iostream>
#include <windows.h>
#include <shlwapi.h>
#include <locale> 
#include <string>
#include <vector>

// 链接路径处理库（shlwapi.lib），确保编译时能找到相关函数实现
#pragma comment(lib, "shlwapi.lib")

namespace DllUtils {
    /**
     * @brief 初始化本地化设置（仅执行一次，线程安全）
     * @note C++11 后 static 变量初始化是线程安全的，自动保证单例执行
     *       作用：设置系统区域设置，确保宽字符输出、路径解析支持中文等多字节字符
     */
    inline void InitLocale() {
        // 用 lambda 表达式初始化 static 变量，实现"初始化+返回"的原子操作
        static const bool kLocaleInitialized = []() {
            setlocale(LC_ALL, "");  // 设置为系统默认区域设置
            return true;
            }();
    }

    /**
     * @brief 拼接基础目录和相对路径，生成完整路径
     * @param baseDir [in] 基础目录（宽字符，绝对路径/相对路径均可，非空）
     * @param relativePath [in] 相对路径（宽字符，相对于 baseDir 的路径，非空）
     * @return 拼接后的完整宽字符路径；失败返回空字符串
     * @note 1. 用 std::vector<wchar_t> 分配堆内存，避免栈空间溢出（支持长路径，最大 32767 字符）
     *       2. 依赖 Windows API PathCombineW 实现路径拼接，自动处理分隔符（/ 或 \）
     */
    inline std::wstring CombinePaths(const wchar_t* baseDir, const wchar_t* relativePath) {
        // 参数校验：基础目录为空（nullptr 或空字符串）
        if (!baseDir || *baseDir == L'\0') {
            std::wcout << L"\033[31m[ERROR]\033[0m [InitDll] 路径拼接失败：基础目录为空" << std::endl;
            return L"";
        }
        // 参数校验：相对路径为空（nullptr 或空字符串）
        if (!relativePath || *relativePath == L'\0') {
            std::wcout << L"\033[31m[ERROR]\033[0m [InitDll] 路径拼接失败：相对路径为空" << std::endl;
            return L"";
        }

        // 堆分配路径缓冲区（32767 字符，支持长路径，vector 自动初始化并释放内存）
        std::vector<wchar_t> fullPathBuffer(32767, L'\0');
        // 调用 Windows API 拼接路径：PathCombineW(输出缓冲区, 基础目录, 相对路径)
        if (!PathCombineW(fullPathBuffer.data(), baseDir, relativePath)) {
            // 拼接失败，打印系统错误信息（GetLastError() 获取错误码）
            DWORD errorCode = GetLastError();
            std::wcout << L"\033[31m[ERROR]\033[0m [InitDll] 路径拼接失败，错误码：" << errorCode << std::endl;
            return L"";
        }

        // 将缓冲区转换为 std::wstring（自动忽略末尾的空字符）
        return std::wstring(fullPathBuffer.data());
    }

    /**
     * @brief 获取当前进程的工作目录（绝对路径）
     * @return 宽字符格式的当前工作目录；失败返回空字符串
     * @note 1. 堆分配缓冲区，支持长路径（最大 32767 字符）
     *       2. 依赖 Windows API GetCurrentDirectoryW，获取进程启动时的工作目录
     */
    inline std::wstring GetCurrentDirectory() {
        // 堆分配缓冲区（vector 管理内存，避免栈溢出）
        std::vector<wchar_t> currentDirBuffer(32767, L'\0');
        // 调用 Windows API 获取工作目录：GetCurrentDirectoryW(缓冲区大小, 输出缓冲区)
        // 注意：第二个参数需传 buffer.data()（获取 vector 底层数组指针），而非 vector 本身
        if (!GetCurrentDirectoryW(static_cast<DWORD>(currentDirBuffer.size()), currentDirBuffer.data())) {
            DWORD errorCode = GetLastError();
            std::wcout << L"\033[31m[ERROR]\033[0m [InitDll] 获取当前工作目录失败，错误码：" << errorCode << std::endl;
            return L"";
        }

        // 转换为 std::wstring 返回
        return std::wstring(currentDirBuffer.data());
    }

    /**
     * @brief 解析 DLL 的完整路径（当前工作目录 + 相对路径），并验证路径有效性
     * @param relativePath [in] DLL 相对于当前工作目录的路径（宽字符）
     * @return 验证通过的 DLL 完整宽字符路径；失败返回空字符串
     * @note 流程：初始化本地化 → 获取当前工作目录 → 拼接路径 → 验证路径存在性
     */
    inline std::wstring GetDllPath(const wchar_t* relativePath) {
        InitLocale();  // 确保本地化已初始化，支持中文路径

        // 步骤1：获取当前工作目录
        std::wstring currentDir = GetCurrentDirectory();
        if (currentDir.empty()) {
            std::wcout << L"\033[31m[ERROR]\033[0m [InitDll] 获取当前工作目录失败，无法解析 DLL 路径" << std::endl;
            return L"";
        }

        // 步骤2：拼接当前工作目录和相对路径
        std::wstring fullPath = CombinePaths(currentDir.c_str(), relativePath);
        if (fullPath.empty()) {
            std::wcout << L"\033[31m[ERROR]\033[0m [InitDll] 拼接 DLL 路径失败" << std::endl;
            return L"";
        }

        // 步骤3：验证拼接后的路径是否存在（文件/目录均可）
        if (!PathFileExistsW(fullPath.c_str())) {
            std::wcout << L"\033[31m[ERROR]\033[0m [InitDll] 路径不存在：" << fullPath << std::endl;
            return L"";
        }

        // 调试输出：打印当前工作目录和最终 DLL 路径
        std::wcout << L"[InitDll] 当前工作目录: " << currentDir << std::endl;
        std::wcout << L"[InitDll] DLL 完整路径: " << fullPath << std::endl;

        return fullPath;
    }

    /**
     * @brief 初始化 DLL 搜索目录环境（设置默认搜索规则）
     * @return 初始化成功返回 true；失败返回 false
     * @note 作用：设置 DLL 搜索路径规则为 "默认目录"（系统目录、当前目录、PATH 环境变量等）
     *       必须在添加自定义 DLL 目录前调用，确保搜索规则生效
     */
    inline bool InitAddDllDir() {
        InitLocale();  // 初始化本地化，避免后续路径处理乱码

        // 设置默认 DLL 搜索目录规则：LOAD_LIBRARY_SEARCH_DEFAULT_DIRS
        if (!SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS)) {
            DWORD errorCode = GetLastError();
            std::wcout << L"\033[31m[ERROR]\033[0m [InitDll] 设置默认 DLL 搜索路径失败，错误码：" << errorCode << std::endl;
            return false;
        }

        std::wcout << L"[InitDll] DLL 搜索环境初始化成功" << std::endl;
        return true;
    }

    /**
     * @brief 添加自定义 DLL 搜索目录（Windows API：AddDllDirectory）
     * @param dir [in] 要添加的 DLL 目录路径（宽字符，相对路径/绝对路径均可）
     * @return 添加成功返回 true；失败返回 false
     * @note 流程：参数校验 → 解析完整路径 → 调用 API 添加目录
     *       依赖 GetDllPath 解析路径，自动处理路径拼接和有效性验证
     */
    inline bool AddDllDir(const wchar_t* dir) {
        // 参数校验：目录路径为空（nullptr 或空字符串）
        if (!dir || *dir == L'\0') {
            std::wcout << L"\033[31m[ERROR]\033[0m [InitDll] 添加 DLL 目录失败：目录路径为空" << std::endl;
            return false;
        }

        // 解析目录完整路径（自动验证路径存在性）
        std::wstring dllDir = GetDllPath(dir);
        if (dllDir.empty()) {
            std::wcout << L"\033[31m[ERROR]\033[0m [InitDll] 获取有效 DLL 目录失败，跳过添加" << std::endl;
            return false;
        }

        // 调用 Windows 全局 API 添加 DLL 目录（:: 明确指定全局命名空间，避免冲突）
        if (!::AddDllDirectory(dllDir.c_str())) {
            DWORD errorCode = GetLastError();
            std::wcout << L"\033[31m[ERROR]\033[0m [InitDll] 添加 DLL 目录失败：" << dllDir << L"，错误码：" << errorCode << std::endl;
            return false;
        }

        std::wcout << L"[InitDll] 成功添加 DLL 目录：" << dllDir << std::endl;
        return true;
    }

} // namespace DllUtils