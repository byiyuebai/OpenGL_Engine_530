#pragma once
#include <vector>
#include <glm/glm.hpp>
class AtlasMipmap {
private:

public:
	AtlasMipmap(int levels,int channels,unsigned char* data, glm::ivec2 tileCount,glm::ivec2 tileSize);
	void setMipmap(int channels, glm::ivec2 tileCount, glm::ivec2 tileSize);
	void setTile(int channels, int level, glm::ivec2 pos);
	unsigned char getPixel(unsigned char* data, glm::ivec2 size, int channels, int x, int y, int c);
	void setPixel(unsigned char* data, glm::ivec2 size, int channels, int x, int y, int c, unsigned char value);
	~AtlasMipmap();

	int levels;
	std::vector<glm::ivec2> levelSizes;
	std::vector<unsigned char*> mipData;
};