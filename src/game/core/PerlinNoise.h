#include <glm/vec3.hpp>
#include <random>
#include <cmath>
#include <array>
// ========== 手动实现柏林噪声核心 ==========
class PerlinNoise {
private:
    // 置换表（经典Perlin噪声的固定排列，保证随机性和重复性）
    std::array<int, 256> permutation = {
        151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
        8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
        35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
        134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
        55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
        18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
        250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
        189,28,42,223,183,170,213,119,248,152,2,44,154,163, 70,221,153,101,155,167,
        43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,
        97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
        107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
    };
    std::array<int, 512> p; // 扩展置换表，避免越界

    // 插值函数（平滑插值，保证噪声连续性）
    inline float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

    // 线性插值
    inline float lerp(float t, float a, float b) { return a + t * (b - a); }

    // 梯度函数（计算向量点积，核心噪声贡献）
    inline float grad(int hash, float x, float y) {
        int h = hash & 7; // 取最后3位，对应8个方向
        float u = h < 4 ? x : y;
        float v = h < 4 ? y : x;
        return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
    }

public:
    inline PerlinNoise(unsigned int seed = 0) {
        // 根据种子打乱置换表（保证不同种子生成不同地形）
        if (seed != 0) {
            std::shuffle(permutation.begin(), permutation.end(), std::mt19937(seed));
        }
        // 扩展置换表（前256和后256相同，避免取模运算）
        for (int i = 0; i < 512; i++) p[i] = permutation[i % 256];
    }

    // 2D噪声采样（核心接口）
    inline float noise(float x, float y) {
        // 1. 找到所在网格的整数坐标
        int X = (int)floor(x) & 255;
        int Y = (int)floor(y) & 255;

        // 2. 计算网格内的局部坐标（0~1）
        x -= floor(x);
        y -= floor(y);

        // 3. 计算平滑插值因子（减少锯齿）
        float u = fade(x);
        float v = fade(y);

        // 4. 获取四个角的哈希值
        int A = p[X] + Y;
        int B = p[X + 1] + Y;

        // 5. 双线性插值计算最终噪声值（范围：-1 ~ 1）
        float res = lerp(v,
            lerp(u, grad(p[A], x, y), grad(p[B], x - 1, y)),
            lerp(u, grad(p[A + 1], x, y - 1), grad(p[B + 1], x - 1, y - 1))
        );
        return res;
    }

    // 带八度的噪声（叠加多层噪声，增加细节）
    inline float octaveNoise(float x, float y, int octaves = 2, float persistence = 0.5f) {
        float total = 0.0f;
        float frequency = 1.0f;
        float amplitude = 1.0f;
        float maxValue = 0.0f; // 归一化用

        for (int i = 0; i < octaves; i++) {
            total += noise(x * frequency, y * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= 2.0f;
        }
        return total / maxValue; // 归一化到-1~1
    }
};