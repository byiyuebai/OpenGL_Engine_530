#include "texture_manager.h"
#include <stdexcept>
#include <iostream>
#include "loadTextureAtlas.h"
TextureManager* TextureManager::instance = nullptr;
std::mutex TextureManager::mtx;

Texture* TextureManager::loadTexture(const std::string& file_path, TextureType type) {
    // 检查是否已加载
    auto it = textures_.find(file_path);
    if (it != textures_.end()) {
        return it->second;
    }
	std::cout << "[texture_manager] Loading texture: " << file_path << std::endl;
    // 如果没加载则尝试加载纹理
    Texture* raw_texture = new Texture(file_path, type);

    if(raw_texture==nullptr) {
        std::wcerr << L"\033[31m[ERROR]\033[0m [texture_manager] Failed to load texture: " << file_path.c_str() << std::endl;
    }
    textures_.emplace(file_path, raw_texture);

    return raw_texture;
}

TextureAtlas* TextureManager::loadTextureAtlas(const std::string& file_path)
{
    // 检查是否已加载
    auto it = textures_.find(file_path);
    if (it != textures_.end()) {
        return static_cast<TextureAtlas*>(it->second);
    }
    std::cout << "[texture_manager] Loading textureAtlas: " << file_path << std::endl;
    // 如果没加载则尝试加载纹理
    TextureAtlas* raw_texture = LoadTextureAtlas::getInstance()->loadTextureAtlas(file_path);

    if (raw_texture == nullptr) {
        std::wcerr << L"\033[31m[ERROR]\033[0m [texture_manager] Failed to load textureAtlas: " << file_path.c_str() << std::endl;
    }

    textures_.emplace(file_path, raw_texture);

    return raw_texture;
}

void TextureManager::unloadTexture(const std::string& file_path)
{
    auto it = textures_.find(file_path);
    if (it != textures_.end()) {
        it->second->Clean();
        delete it->second; // 释放纹理资源
        textures_.erase(it); // 从管理器中移除
	}
}

void TextureManager::clearTextures()
{
    for (auto& pair : textures_) {
        pair.second->Clean();
        delete pair.second;
    }
    textures_.clear();
}
