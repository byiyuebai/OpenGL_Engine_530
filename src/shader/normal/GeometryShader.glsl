#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 worldPos;
    vec3 worldNormal;
} gs_in[];

uniform mat4 view;
uniform mat4 projection;
uniform float normalLength = 0.1;

out vec3 color; // 传给片段着色器的颜色

void main() {
    for(int i = 0; i < 3; i++) {
        color = vec3(1.0, 0.0, 0.0); // 法线固定为红色
        
        // 原顶点位置
        gl_Position = projection * view * vec4(gs_in[i].worldPos, 1.0);
        EmitVertex();
        
        // 法线末端位置
        vec3 normalEndPos = gs_in[i].worldPos + gs_in[i].worldNormal * normalLength;
        gl_Position = projection * view * vec4(normalEndPos, 1.0);
        EmitVertex();
        
        EndPrimitive();
    }
}