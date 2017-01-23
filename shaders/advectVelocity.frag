#version 400

uniform sampler2D velocity1;
uniform sampler2D Ink;
in vec2 pos;
layout(location=0) out vec4 velocity0;

void main()
{
	float dt = 1.0/100;

	vec2 FragPos = pos - dt*(texture(velocity1,pos).xy);

	velocity0 = vec4(texture(Ink, FragPos).xy , 0 ,1);

//	velocity0 = texture(Ink, pos);
}