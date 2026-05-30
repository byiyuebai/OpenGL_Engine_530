#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 Normal;
out vec2 TexCoord;

uniform mat4 projection;

uniform mat4 view; //这玩意仅需要传入3x3矩阵(去除平移)

uniform vec3 chunkRelativePos;

void main()
{
    vec3 finalPos = aPos + chunkRelativePos;

    gl_Position = projection * view * vec4(finalPos, 1.0);

    Normal = aNormal;
    TexCoord = aTexCoord;
}