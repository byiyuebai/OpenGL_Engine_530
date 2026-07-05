#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;



out vec4 vertexColor;

uniform mat4 model;

layout (std140) uniform Proj {
    mat4 projection;
};
layout (std140) uniform View {
    mat4 view;
};
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexColor = aColor;
}