#version 400

uniform sampler2D pressure0;
in vec2 pos;
in vec2 OFFSET;
layout(location=2) out vec4 pressure1;

void main()
{
	pressure1 = texture(pressure0, pos + OFFSET);
}