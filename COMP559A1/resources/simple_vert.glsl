#version 330 compatibility

uniform mat4 P;
uniform mat4 MV;
out vec4 fragColor;

void main()
{
	gl_Position = P * MV * gl_Vertex;
	fragColor = gl_Color;
}
