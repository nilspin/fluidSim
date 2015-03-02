//input vertex data
layout(location=0) in vec3 quad_vertices;

//interpolated output to frag shader
out vec3 color;

void main()
{
	color = vec3(quad_vertices);
}