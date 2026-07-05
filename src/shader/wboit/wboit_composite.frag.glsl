#version 330 core
in vec2 TexCoord;

layout(location = 0) out vec4 FragColor;

uniform sampler2D opaqueTexture;
uniform sampler2D accumulationTexture;
uniform sampler2D revealageTexture;

void main()
{
    // 不透明背景
    vec3 opaqueColor = texture(opaqueTexture, TexCoord).rgb;
    
    // 透明物体的累加数据
    vec4 accum = texture(accumulationTexture, TexCoord);
    
    // 透射率
    float revealage = texture(revealageTexture, TexCoord).r;

    // 混合
    vec3 outColor = mix(accum.rgb / max(accum.a, 1e-5),opaqueColor,  revealage);


//    float dist = abs(gl_FragCoord.x - 1200.0);
//    if (dist < 1.0) {  // 1像素宽的线
//        outColor = vec3(1.0, 1.0, 0.0);
//    }

    FragColor = vec4(outColor, 1.0);
    
    // 调试：显示各个缓冲的值
    // 取消注释下面任意一行来调试
    //FragColor = vec4(opaqueColor, 1.0);                      // 只显示不透明物体
    //FragColor = vec4(revealage, revealage, revealage, 1.0);  // 显示 revealage

    //FragColor = vec4(accum.rgb, 1.0);                          // 显示累加颜色       
    //FragColor = vec4(accum.rgb / max(accum.a, 1e-5), 1.0);
    //FragColor = vec4(accum.aaa, 1.0);                        // 显示累加权重
}