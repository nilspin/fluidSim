uniform sampler2D textureSampler;
in vec3 color;	//interpolated variable from vert shader
out vec3 Color;	//final output to screen

void main()
{
	Color = texture(textureSampler, color);
}