#version 330 compatibility
//layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

out vec2 TexCoords;

uniform mat4 P;
uniform mat4 MV;
out vec4 fragColor;

void main() {
    //gl_Position = P * MV * vec4(vertex.xy, 0.0, 1.0);
    //TexCoords = vertex.zw;
    gl_Position = P * MV * vec4(gl_Vertex.xy, 0.0, 1.0);
    TexCoords = gl_Vertex.zw;
    fragColor = gl_Color;
}