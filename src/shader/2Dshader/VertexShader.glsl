#version 330 core
layout (location = 0) in vec3 aPos; // 位置变量的属性位置值为0
layout (location = 1) in vec3 aNormal; // 法线变量的属性位置值为2
layout (location = 2) in vec2 aTexCoord; // 纹理坐标变量的属性位置值为3


out vec3 Normal;
out vec2 TexCoord;

out float fragDepth;

uniform mat4 model;
void main()
{
    gl_Position = model * vec4(aPos, 1.0);

//    fragDepth= gl_Position.z / gl_Position.w;
//
//    gl_Position.z = 1;

    mat3 normalMatrix = mat3(inverse(transpose(model)));
    Normal = normalMatrix * aNormal;

    TexCoord = aTexCoord;
}