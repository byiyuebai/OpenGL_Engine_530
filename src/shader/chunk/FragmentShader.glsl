#version 330 core


layout (location = 0) out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D Texture1;


vec3 clampColor(vec3 color) {return clamp(color, 0.0, 1.0);}

void main()
{
     vec4 tex1 = texture(Texture1, TexCoord);

     if(tex1.a < 0.1) discard; // 透明度过低的像素直接丢弃

    // ===== 1. 光照参数优化（核心：降低环境光强度，可选：柔化颜色）=====
    vec3 lightDir = normalize(vec3(0.5, 1.0, 1.0));
    float ambientStrength = 0.4;
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 ambient = ambientStrength * lightColor;

    // ===== 2. 漫反射计算（保持不变）=====
    vec3 nNormal = normalize(Normal);
    float diff = max(dot(nNormal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // ===== 3. 纹理采样 + 光照融合（可选：增加gamma校正，让亮度更自然）=====
    vec3 finalColor = clampColor(ambient + diffuse);
    finalColor = pow(finalColor, vec3(1.0/2.2));

    vec4 out1 = tex1 * vec4(finalColor, 1.0);

    FragColor = out1;
}