uniform sampler2D textureSampler;

out vec4 Color;
in vec3 pos;

void main()
{
	Color = texture2D(textureSampler, pos.xy);
}