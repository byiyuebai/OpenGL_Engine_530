#ifndef TEXTURE_VIEWER_H
#define TEXTURE_VIEWER_H

// ==============================
// 头文件引入区（按功能分类）
// ==============================
// Assimp核心库（模型加载）
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// C++标准库（基础功能）
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <stdexcept>
#include <locale>

// Windows系统库（字符编码转换）
#include <windows.h>

// ==============================
// 命名空间与类型定义区
// ==============================
// 命名空间隔离，避免命名冲突
namespace TextureViewer {

    // 宽字符字符串类型别名（简化代码，统一命名风格）
    using WideString = std::wstring;

    // ==============================
    // 核心数据结构定义（注释增强）
    // ==============================
    /**
     * @brief 单张纹理的信息结构体（全宽字符存储）
     * @details 包含纹理文件路径和类型信息，提供类型转字符串的便捷接口
     */
    struct TextureInfo {
        WideString filePath;              // 纹理文件完整路径（宽字符，支持中文）
        aiTextureType textureType;        // 纹理类型（漫反射/高光/法线等）

        /**
         * @brief 将纹理类型转换为可读的宽字符字符串
         * @return 纹理类型的中文+英文标识字符串
         */
        WideString getTextureTypeString() const {
            switch (textureType) {
            case aiTextureType_DIFFUSE:    return L"漫反射 (DIFFUSE)";
            case aiTextureType_SPECULAR:   return L"高光 (SPECULAR)";
            case aiTextureType_NORMALS:    return L"法线 (NORMALS)";
            case aiTextureType_HEIGHT:     return L"高度 (HEIGHT)";
            case aiTextureType_AMBIENT:    return L"环境光 (AMBIENT)";
            default: {
                WideString typeNumber = std::to_wstring(static_cast<int>(textureType));
                return L"未知类型 (" + typeNumber + L")";
            }
            }
        }
    };

    /**
     * @brief 单个网格及其关联纹理的结构体
     * @details 存储网格索引、名称，以及该网格绑定的所有纹理信息
     */
    struct MeshTextureData {
        unsigned int meshIndex = 0;                // 网格在场景中的索引（从0开始）
        WideString meshName;                       // 网格名称（宽字符，支持中文）
        std::vector<TextureInfo> textureList;      // 该网格关联的所有纹理列表

        /**
         * @brief 打印当前网格的纹理信息（控制台输出，宽字符兼容）
         */
        void printMeshTextureInfo() const {
            std::wcout << L"=== 网格 " << meshIndex << L" (" << meshName << L") ===" << std::endl;

            if (textureList.empty()) {
                std::wcout << L"  无关联纹理" << std::endl;
                return;
            }

            for (size_t i = 0; i < textureList.size(); ++i) {
                const auto& texture = textureList[i];
                std::wcout << L"  纹理 " << (i + 1) << L": "
                    << L"类型=" << texture.getTextureTypeString()
                    << L", 路径=" << texture.filePath << std::endl;
            }
        }
    };

    // ==============================
    // 工具函数（字符编码转换）
    // ==============================
    /**
     * @brief 窄字符串（UTF-8）转宽字符串
     * @param str 输入的UTF-8编码窄字符串
     * @return 转换后的宽字符串（支持中文路径）
     */
    inline WideString ConvertUtf8ToWideString(const std::string& str) {
        if (str.empty()) return L"";

        // 获取转换所需缓冲区大小
        const int bufferSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        wchar_t* buffer = new wchar_t[bufferSize];

        // 执行转换
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, bufferSize);
        WideString wideStr(buffer);

        // 释放内存
        delete[] buffer;
        return wideStr;
    }

    /**
     * @brief 宽字符串转窄字符串（UTF-8）
     * @param wideStr 输入的宽字符串
     * @return 转换后的UTF-8编码窄字符串
     */
    inline std::string ConvertWideStringToUtf8(const WideString& wideStr) {
        if (wideStr.empty()) return "";

        // 获取转换所需缓冲区大小
        const int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        char* buffer = new char[bufferSize];

        // 执行转换
        WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, buffer, bufferSize, nullptr, nullptr);
        std::string utf8Str(buffer);

        // 释放内存
        delete[] buffer;
        return utf8Str;
    }

    // ==============================
    // 核心功能类（模型纹理解析）
    // ==============================
    /**
     * @brief 3D模型纹理查看器核心类
     * @details 基于Assimp库解析模型文件，提取所有网格关联的纹理信息，
     *          全程使用宽字符处理，支持中文路径和名称
     */
    class ModelTextureViewer {
    private:
        // 核心成员变量（添加注释，统一命名风格）
        WideString m_modelFilePath;                // 模型文件路径（宽字符，支持中文）
        const aiScene* m_loadedScene = nullptr;     // Assimp加载的场景指针（nullptr表示未加载）
        Assimp::Importer m_importer;               // Assimp导入器（与类生命周期绑定）
        std::vector<MeshTextureData> m_meshTextureList; // 网格-纹理映射结果列表
        bool m_isModelLoaded = false;              // 模型加载状态标记

        /**
         * @brief 从材质中提取指定类型的纹理（内部辅助函数）
         * @param material 指向Assimp材质对象的指针
         * @param textureType 要提取的纹理类型
         * @return 提取到的纹理信息列表
         */
        std::vector<TextureInfo> ExtractTexturesFromMaterial(aiMaterial* material, aiTextureType textureType) const {
            std::vector<TextureInfo> textureList;
            const unsigned int textureCount = material->GetTextureCount(textureType);

            for (unsigned int i = 0; i < textureCount; ++i) {
                aiString texturePathStr;
                material->GetTexture(textureType, i, &texturePathStr);

                // 转换为宽字符纹理信息
                TextureInfo textureInfo;
                textureInfo.filePath = ConvertUtf8ToWideString(texturePathStr.C_Str());
                textureInfo.textureType = textureType;

                textureList.push_back(textureInfo);
            }

            return textureList;
        }

        /**
         * @brief 加载并解析模型纹理（内部核心逻辑）
         * @throws std::runtime_error 文件不存在/加载失败/数据异常时抛出
         */
        void LoadAndParseModelTextures() {
            // 步骤1：验证文件路径有效性
            const std::filesystem::path modelFsPath(m_modelFilePath);
            if (!std::filesystem::exists(modelFsPath)) {
                throw std::runtime_error("模型文件不存在: " + ConvertWideStringToUtf8(m_modelFilePath));
            }
            if (!std::filesystem::is_regular_file(modelFsPath)) {
                throw std::runtime_error("路径不是有效文件: " + ConvertWideStringToUtf8(m_modelFilePath));
            }

            // 步骤2：使用Assimp加载模型（宽路径转UTF-8窄路径）
            const std::string utf8ModelPath = ConvertWideStringToUtf8(m_modelFilePath);
            m_loadedScene = m_importer.ReadFile(
                utf8ModelPath,
                aiProcess_Triangulate |        // 三角化：将多边形转为三角形
                aiProcess_GenSmoothNormals |   // 生成平滑法线：优化光照计算
                aiProcess_FlipUVs |            // 翻转UV：适配OpenGL纹理坐标
                aiProcess_CalcTangentSpace |   // 计算切线空间：支持法线贴图
                aiProcess_ValidateDataStructure // 验证数据结构：避免加载损坏模型
            );

            // 步骤3：检查模型加载结果
            if (!m_loadedScene || (m_loadedScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !m_loadedScene->mRootNode) {
                throw std::runtime_error("Assimp加载模型失败: " + std::string(m_importer.GetErrorString()));
            }

            // 步骤4：缓存材质纹理（避免重复解析）
            std::unordered_map<unsigned int, std::vector<TextureInfo>> materialTextureCache;
            for (unsigned int materialIndex = 0; materialIndex < m_loadedScene->mNumMaterials; ++materialIndex) {
                aiMaterial* material = m_loadedScene->mMaterials[materialIndex];
                std::vector<TextureInfo> materialTextures;

                // 批量提取主流纹理类型
                auto addTexturesToList = [&](aiTextureType type) {
                    auto extractedTextures = ExtractTexturesFromMaterial(material, type);
                    materialTextures.insert(
                        materialTextures.end(),
                        extractedTextures.begin(),
                        extractedTextures.end()
                    );
                    };

                addTexturesToList(aiTextureType_DIFFUSE);
                addTexturesToList(aiTextureType_SPECULAR);
                addTexturesToList(aiTextureType_NORMALS);
                addTexturesToList(aiTextureType_HEIGHT);
                addTexturesToList(aiTextureType_AMBIENT);

                materialTextureCache[materialIndex] = materialTextures;
            }

            // 步骤5：建立网格与纹理的关联映射
            m_meshTextureList.clear();
            for (unsigned int meshIndex = 0; meshIndex < m_loadedScene->mNumMeshes; ++meshIndex) {
                aiMesh* mesh = m_loadedScene->mMeshes[meshIndex];
                MeshTextureData meshTextureData;

                // 填充网格基础信息
                meshTextureData.meshIndex = meshIndex;
                meshTextureData.meshName = ConvertUtf8ToWideString(mesh->mName.C_Str());

                // 关联材质纹理
                const unsigned int materialIndex = mesh->mMaterialIndex;
                if (materialTextureCache.find(materialIndex) != materialTextureCache.end()) {
                    meshTextureData.textureList = materialTextureCache[materialIndex];
                }

                m_meshTextureList.push_back(meshTextureData);
            }

            // 标记加载成功
            m_isModelLoaded = true;
        }

    public:
        /**
         * @brief 构造函数（宽字符路径版本，推荐使用）
         * @param modelFilePath 模型文件的宽字符路径（支持中文）
         */
        explicit ModelTextureViewer(const WideString& modelFilePath)
            : m_modelFilePath(modelFilePath) {
            try {
                LoadAndParseModelTextures();
            }
            catch (const std::exception& e) {
                std::wcerr << L"初始化失败: " << ConvertUtf8ToWideString(e.what()) << std::endl;
                m_isModelLoaded = false;
            }
        }

        /**
         * @brief 构造函数（窄字符路径版本，自动转换编码）
         * @param modelFilePath 模型文件的UTF-8窄字符路径
         */
        explicit ModelTextureViewer(const std::string& modelFilePath)
            : m_modelFilePath(ConvertUtf8ToWideString(modelFilePath)) {
            try {
                LoadAndParseModelTextures();
            }
            catch (const std::exception& e) {
                std::wcerr << L"初始化失败: " << ConvertUtf8ToWideString(e.what()) << std::endl;
                m_isModelLoaded = false;
            }
        }

        // 析构函数（空实现，Assimp内部自动释放资源）
        ~ModelTextureViewer() = default;

        // 禁止拷贝（避免Assimp资源重复释放）
        ModelTextureViewer(const ModelTextureViewer&) = delete;
        ModelTextureViewer& operator=(const ModelTextureViewer&) = delete;

        // 允许移动（支持临时对象传递）
        ModelTextureViewer(ModelTextureViewer&&) = default;
        ModelTextureViewer& operator=(ModelTextureViewer&&) = default;

        // ==============================
        // 公共接口（功能清晰化）
        // ==============================
        /**
         * @brief 获取模型加载状态
         * @return true=加载成功，false=加载失败
         */
        bool IsModelLoaded() const {
            return m_isModelLoaded;
        }

        /**
         * @brief 获取所有网格的纹理信息
         * @return 网格-纹理映射列表（const引用，避免拷贝）
         * @throws std::runtime_error 模型未加载时抛出
         */
        const std::vector<MeshTextureData>& GetMeshTextureList() const {
            if (!m_isModelLoaded) {
                throw std::runtime_error("模型未加载成功，无法获取纹理信息");
            }
            return m_meshTextureList;
        }

        /**
         * @brief 打印所有网格的纹理信息（控制台输出）
         * @details 宽字符输出，支持中文路径和名称
         */
        void PrintAllTextureInfo() const {
            if (!m_isModelLoaded) {
                std::wcerr << L"无法打印：模型未加载成功" << std::endl;
                return;
            }

            std::wcout << L"\n===== 模型纹理信息汇总 =====" << std::endl;
            std::wcout << L"模型路径: " << m_modelFilePath << std::endl;
            std::wcout << L"总网格数: " << m_meshTextureList.size() << std::endl;

            for (const auto& meshData : m_meshTextureList) {
                meshData.printMeshTextureInfo();
            }
        }

        /**
         * @brief 获取模型中所有唯一纹理（去重）
         * @return 去重后的纹理信息列表
         * @details 按「路径+类型」去重，避免重复纹理
         * @throws std::runtime_error 模型未加载时抛出
         */
        std::vector<TextureInfo> GetAllUniqueTextures() const {
            if (!m_isModelLoaded) {
                throw std::runtime_error("模型未加载成功，无法获取纹理信息");
            }

            // 按「路径+类型」作为Key去重
            std::unordered_map<std::string, TextureInfo> uniqueTextureMap;
            for (const auto& meshData : m_meshTextureList) {
                for (const auto& texture : meshData.textureList) {
                    const std::string uniqueKey =
                        ConvertWideStringToUtf8(texture.filePath) + "_" +
                        std::to_string(static_cast<int>(texture.textureType));

                    if (uniqueTextureMap.find(uniqueKey) == uniqueTextureMap.end()) {
                        uniqueTextureMap[uniqueKey] = texture;
                    }
                }
            }

            // 转换为列表返回
            std::vector<TextureInfo> uniqueTextureList;
            for (const auto& pair : uniqueTextureMap) {
                uniqueTextureList.push_back(pair.second);
            }
            return uniqueTextureList;
        }
    };

} // namespace TextureViewer

#endif