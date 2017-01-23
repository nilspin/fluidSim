#version 400

in vec3 position;
out vec2 pos;
out vec3 Color;

void main() {
	Color = vec3(1,0,0);
	pos = (vec2(position) + vec2(1,1)) / 2.0;//vec2(position);//
	gl_Position = vec4(position, 1.0);
}
