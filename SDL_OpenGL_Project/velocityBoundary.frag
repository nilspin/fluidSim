#version 400

uniform sampler2D velocity0;
in vec2 pos;
in vec2 OFFSET;
layout(location=1) out vec4 velocity1;

void main()
{
	vec2 off = vec2(OFFSET.x/640, OFFSET.y/480);
	velocity1 = texture(velocity0, pos + off)*(-1.0);
}