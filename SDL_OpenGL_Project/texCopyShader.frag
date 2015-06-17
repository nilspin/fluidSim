#version 400

in vec2 pos;
uniform sampler2D velocity1;
layout(location=0) out vec4 velocity0;

void main()
{
	velocity0 = texture(velocity1,pos);
}