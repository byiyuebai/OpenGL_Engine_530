#version 330 core
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D overTexture;

uniform vec2 pixSize;
uniform float i;

vec2 offsets[9] = vec2[](
        vec2(-pixSize.x,  pixSize.y), // 左上
        vec2( 0.0f,    pixSize.y), // 正上
        vec2( pixSize.x,  pixSize.y), // 右上
        vec2(-pixSize.x,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( pixSize.x,  0.0f),   // 右
        vec2(-pixSize.x, -pixSize.y), // 左下
        vec2( 0.0f,   -pixSize.y), // 正下
        vec2( pixSize.x, -pixSize.y)  // 右下
    );

float kernel[9] = float[](
    1.0 / 8, 2.0 / 8, 1.0 / 8,
    2.0 / 8, 4.0 / 8, 2.0 / 8,
    1.0 / 8, 2.0 / 8, 1.0 / 8  
);

void main() {

    vec3 aout = vec3(0.0);
    if(gl_FragCoord.x <400){
    vec3 colors[9];
    for(int i = 0; i < 9; i++)
    {
        colors[i] = vec3(texture(overTexture, TexCoord + offsets[i]));
    }

    for(int i = 0; i < 9; i++)
        aout += colors[i] * kernel[i];
    }
    else{
        aout = vec3(texture(overTexture, TexCoord));
    }

    FragColor = vec4(aout, 1.0); // 取中心像素
}