#version 400

layout(location=0) in vec3 quad_vertices;
out vec2 pos;

void main()
{
	pos = (quad_vertices.xy + vec2(1,1))/2.0;//quad_vertices.xy; //
	gl_Position = vec4(quad_vertices,1.0);
}