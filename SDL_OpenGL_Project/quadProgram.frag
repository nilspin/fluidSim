#version 400

//uniform sampler2D velocity0;
//uniform sampler2D pressure0;
uniform sampler2D texturesampler;

out vec4 Color;
in vec2 pos;

void main()
{
//for white color
//	vec4 temp = texture(texturesampler, pos);
//	Color = abs(vec4(vec3(temp.x + temp.y / 2.0),1.0));

//	Color = vec4((texture(pressure0, pos)).x,(texture(velocity0, pos)*1.5+0.5).xy,1.0);

//For viewing divergence
//	float div = texture(texturesampler,pos).x;
//	Color = vec4(div,div,div,1.0);

//For normal viewing
	Color = abs(texture(texturesampler,pos));
}