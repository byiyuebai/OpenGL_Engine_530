#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord; // 保留但不用，避免属性位置冲突

// 输出到几何着色器的数据
out VS_OUT {
    vec3 worldPos;    // 世界空间顶点位置
    vec3 worldNormal; // 世界空间法线
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    // 计算世界空间位置和法线（供几何着色器使用）
    vs_out.worldPos = vec3(model * vec4(aPos, 1.0));
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.worldNormal = normalize(normalMatrix * aNormal);
    
    // 顶点位置仅用于几何着色器计算，这里gl_Position可以先不赋值（几何着色器会重算）
    gl_Position = vec4(aPos, 1.0);
}