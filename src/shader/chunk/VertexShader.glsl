#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 Normal;
out vec2 TexCoord;

uniform vec3 chunkRelativePos;

layout (std140) uniform Proj {
    mat4 projection;
};
layout (std140) uniform View {
    mat4 view;
};

void main()
{
    vec3 finalPos = aPos + chunkRelativePos;

    gl_Position = projection * view * vec4(finalPos, 1.0);

    Normal = aNormal;
    TexCoord = aTexCoord;
}