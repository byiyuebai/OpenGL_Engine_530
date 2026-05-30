#version 330 core
in vec3 Normal;  
in vec2 TexCoord;

layout(location = 0) out vec4 Revealage;

uniform sampler2D texture_base;

float computeWeight(float z, float alpha) {
    float depthScale = pow(1.0 - clamp(z, 0.0, 1.0), 3.0);
    return alpha * depthScale * (1.0 + 10.0 * alpha);
}

void main()
{
    vec4 texColor = texture(texture_base, TexCoord);
    float alpha = texColor.a;
    
    if(alpha < 0.05) discard;
    
    // 计算当前片元的权重
    float weight = computeWeight(gl_FragCoord.z, alpha);
    
    // Revealage存储的是 (1 - weight)，通过乘法混合累乘
    // 混合模式需要设置为 GL_ZERO, GL_ONE_MINUS_SRC_COLOR
    Revealage = vec4(weight);
}