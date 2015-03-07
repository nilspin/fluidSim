in vec3 position;
in vec3 color;
out vec3 Color;

uniform mat4 MVP;

void main() {
	Color = color;
	gl_Position = MVP*vec4(position, 1.0);
}
