layout(location=0) in vec3 quad_vertices;
out vec3 pos;

void main()
{
	pos = vec3(quad_vertices);
	gl_Position = vec4(quad_vertices,1.0);
}