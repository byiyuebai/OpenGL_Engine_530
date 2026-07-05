#version 330 core

in vec2 TexCoord;

layout(location = 0) out vec4 Revealage;

uniform sampler2D texture_base;

void main()
{
    float alpha = texture(texture_base, TexCoord).a;
    
    if(alpha < 0.01) discard;
    
    Revealage = vec4(alpha);
}