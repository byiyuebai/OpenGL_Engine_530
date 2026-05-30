#version 330 core
out vec4 FragColor;
in vec3 color; // 接收几何着色器的颜色

void main()
{
    FragColor = vec4(color, 1.0); // 输出红色，不透明
}