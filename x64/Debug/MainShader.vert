uniform float alpha;
in vec3 position;
in vec3 color;//uniform vec3 color;//
out vec3 Color;
//out vec2 texCoord;

uniform mat4 MVP;

void main() {
	Color = color*alpha;
	gl_Position = MVP*vec4(position, 1.0);
}
