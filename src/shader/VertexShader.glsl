#version 330 core
layout (location = 0) in vec3 aPos; // 位置变量的属性位置值为0
layout (location = 1) in vec3 aNormal; // 法线变量的属性位置值为2
layout (location = 2) in vec2 aTexCoord; // 纹理坐标变量的属性位置值为3


out vec3 Normal;
out vec2 TexCoord;


uniform mat4 projection;
uniform mat4 view;


//uniform模型矩阵时应当位移->缩放->旋转
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    mat3 normalMatrix = mat3(inverse(transpose(model)));
    Normal = normalMatrix * aNormal;

    TexCoord = aTexCoord;
}