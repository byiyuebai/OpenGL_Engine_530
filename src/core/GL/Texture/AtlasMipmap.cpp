#include "AtlasMipmap.h"
#include <algorithm>
AtlasMipmap::AtlasMipmap(int levels, int channels, unsigned char* data, glm::ivec2 tileCount, glm::ivec2 tileSize) : levels(levels){

	levelSizes.reserve(levels + 1);
	glm::ivec2 currentSize = tileCount * tileSize;
	levelSizes.emplace_back(currentSize);
	for (int i = 0; i < levels; ++i) {
		currentSize = glm::ivec2(std::max(1, currentSize.x / 2), std::max(1, currentSize.y / 2));
		levelSizes.emplace_back(currentSize);
	}

	mipData.reserve(levels + 1);
	mipData.emplace_back(data);
	setMipmap(channels, tileCount, tileSize);
}

// 生成 mipmap 主函数
void AtlasMipmap::setMipmap(int channels, glm::ivec2 tileCount, glm::ivec2 tileSize) {
    // 为每个 mip 层级分配内存并计算像素值
    for (int i = 1; i <= levels; ++i) {
        // 计算当前层级的像素总数
        size_t pixelCount = (size_t)levelSizes[i].x * levelSizes[i].y * channels;
        // 分配内存
        unsigned char* levelData = new unsigned char[pixelCount]();
        mipData.push_back(levelData);

        // 遍历当前层级的所有像素，计算插值
        for (int y = 0; y < levelSizes[i].y; ++y) {
            for (int x = 0; x < levelSizes[i].x; ++x) {
                setTile(channels, i, glm::ivec2(x, y));
            }
        }
    }
}

void AtlasMipmap::setTile(int channels, int level, glm::ivec2 pos) {
    glm::ivec2 prevSize = levelSizes[level - 1];
    glm::ivec2 currSize = levelSizes[level];
    unsigned char* prevData = mipData[level - 1];
    unsigned char* currData = mipData[level];

    // 计算当前像素对应上级纹理的四个像素坐标（整数映射，无浮点）
    int x0 = pos.x * 2;
    int y0 = pos.y * 2;
    int x1 = std::min(x0 + 1, prevSize.x - 1); // 边界保护
    int y1 = std::min(y0 + 1, prevSize.y - 1);

    // 对每个颜色通道计算四像素等权平均
    for (int c = 0; c < channels; ++c) {
        // 获取四个像素的通道值
        float p00 = (float)getPixel(prevData, prevSize, channels, x0, y0, c);
        float p01 = (float)getPixel(prevData, prevSize, channels, x0, y1, c);
        float p10 = (float)getPixel(prevData, prevSize, channels, x1, y0, c);
        float p11 = (float)getPixel(prevData, prevSize, channels, x1, y1, c);

        // 等权平均：四个值相加除以4，四舍五入
        float avg = (p00 + p01 + p10 + p11) / 4.0f;
        setPixel(currData, currSize, channels, pos.x, pos.y, c, (unsigned char)std::round(avg));
    }
}

// 获取指定位置的像素值（处理边界）
unsigned char AtlasMipmap::getPixel(unsigned char* data, glm::ivec2 size, int channels, int x, int y, int c) {
    // 边界检查：确保坐标在有效范围内
    x = std::clamp(x, 0, size.x - 1);
    y = std::clamp(y, 0, size.y - 1);
    c = std::clamp(c, 0, channels - 1);

    // 计算像素在数组中的索引：y * 宽度 * 通道数 + x * 通道数 + 通道索引
    size_t index = static_cast<size_t>(y) * size.x * channels + static_cast<size_t>(x) * channels + c;
    return data[index];
}

// 设置指定位置的像素值
void AtlasMipmap::setPixel(unsigned char* data, glm::ivec2 size, int channels, int x, int y, int c, unsigned char value) {
    // 边界检查
    if (x < 0 || x >= size.x || y < 0 || y >= size.y || c < 0 || c >= channels) {
        return;
    }

    // 计算索引并设置值
    size_t index = static_cast<size_t>(y) * size.x * channels + static_cast<size_t>(x) * channels + c;
    data[index] = value;
}

AtlasMipmap::~AtlasMipmap()
{
}