#include "texture_manager.h"
#include <stdexcept>
#include <iostream>
#include "loadTextureAtlas.h"
TextureManager* TextureManager::instance = nullptr;
std::mutex TextureManager::mtx;

Texture* TextureManager::loadTexture(const std::string& file_path, TextureType type) {
	// 检查是否已加载
	auto it = textures_image.find(file_path);
	if (it != textures_image.end()) {
		return it->second;
	}
	std::cout << "[texture_manager] Loading texture: " << file_path << std::endl;
	// 如果没加载则尝试加载纹理
	Texture* raw_texture = new ImageTexture(file_path, type);

	if (raw_texture == nullptr) {
		std::wcerr << L"\033[31m[ERROR]\033[0m [texture_manager] Failed to load texture: " << file_path.c_str() << std::endl;
	}
	textures_image.emplace(file_path, raw_texture);

	return raw_texture;
}

TextureAtlas* TextureManager::loadTextureAtlas(const std::string& file_path)
{
	// 检查是否已加载
	auto it = textures_atlas.find(file_path);
	if (it != textures_atlas.end()) {
		return static_cast<TextureAtlas*>(it->second);
	}
	std::cout << "[texture_manager] Loading textureAtlas: " << file_path << std::endl;
	// 如果没加载则尝试加载纹理
	TextureAtlas* raw_texture = LoadTextureAtlas::getInstance()->loadTextureAtlas(file_path);

	if (raw_texture == nullptr) {
		std::wcerr << L"\033[31m[ERROR]\033[0m [texture_manager] Failed to load textureAtlas: " << file_path.c_str() << std::endl;
	}

	textures_atlas.emplace(file_path, raw_texture);

	return raw_texture;
}

void TextureManager::unloadTexture(const std::string& file_path)
{
	if (file_path.ends_with("json")) {
		auto it_atlas = textures_atlas.find(file_path);
		if (it_atlas != textures_atlas.end()) {
			it_atlas->second->Clean();
			delete it_atlas->second; // 释放纹理资源
			textures_atlas.erase(it_atlas); // 从管理器中移除
		}
	}
	else {
		auto it = textures_image.find(file_path);
		if (it != textures_image.end()) {
			it->second->Clean();
			delete it->second; // 释放纹理资源
			textures_image.erase(it); // 从管理器中移除
		}
	}
}

void TextureManager::clearTextures()
{
	for (auto& pair : textures_image) {
		pair.second->Clean();
		delete pair.second;
	}
	for (auto& pair : textures_atlas) {
		pair.second->Clean();
		delete pair.second;
	}
	textures_image.clear();
	textures_atlas.clear();
}
