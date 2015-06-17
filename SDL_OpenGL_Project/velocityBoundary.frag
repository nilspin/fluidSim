#version 400

uniform sampler2D velocity0;
in vec2 pos;
layout(location=1) out vec4 velocity1;

void main()
{
	velocity1 = texture(velocity0, pos - texture2D(velocity0,pos).xy)*(-1.0);
}