#version 330 core
in vec3 Normal;
in vec2 TexCoord;

layout(location = 0) out vec4 Accumulation;

uniform sampler2D texture_base;

// WBOIT权重函数
float computeWeight(float z, float alpha) {
    // 权重公式：w = alpha * (1 - z)^3 * (1 + 10 * alpha)
    float depthScale = pow(1.0 - clamp(z, 0.0, 1.0), 3.0);
    return alpha * depthScale * (1.0 + 10.0 * alpha);
}

void main()
{
    vec4 texColor = texture(texture_base, TexCoord);
    float alpha = texColor.a;
    
    if(alpha < 0.05) discard; // 忽略几乎透明的像素
    
    // 获取深度（NDC空间，范围0-1）
    float depth = gl_FragCoord.z;
    
    // 计算权重
    float weight = computeWeight(depth, alpha);
    
    // 累加颜色（加权）
    Accumulation = vec4(texColor.rgb * weight, alpha * weight);
    
}