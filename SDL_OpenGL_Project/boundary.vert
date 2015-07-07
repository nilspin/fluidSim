#version 400

in vec3 position;
in vec2 offset;
out vec2 pos;
out vec2 OFFSET;

void main() {
	OFFSET = offset/vec2(640,480);
	pos = (vec2(position) + vec2(1,1)) / 2.0;
	gl_Position = vec4(position, 1.0);
}
