in vec3 position;
out vec3 Color;

uniform mat4 MVP;

void main() {
	Color = vec3(1,0,0);
	gl_Position = vec4(position, 1.0);
}
