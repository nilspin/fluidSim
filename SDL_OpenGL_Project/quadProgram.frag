//uniform sampler2D velocity0;
//uniform sampler2D pressure0;
uniform sampler2D texturesampler;

out vec4 Color;
in vec2 pos;

void main()
{
//	Color = textureOffset(texturesampler,pos,ivec2(0,0));

//	Color = vec4((texture(pressure0, pos)).x,(texture(velocity0, pos)*1.5+0.5).xy,1.0);

//For viewing divergence
//	float div = texture(texturesampler,pos).x;
//	Color = vec4(div,div,div,1.0);

//For normal viewing
	Color = texture(texturesampler,pos);
}