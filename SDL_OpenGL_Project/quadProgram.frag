uniform sampler2D textureSampler;

out vec4 Color;
in vec3 pos;

void main()
{
//	Color = textureOffset(textureSampler,pos.xy,vec2(0,0));
	Color = texture2D(textureSampler, pos.xy);
}