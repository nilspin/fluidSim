in vec3 Color;
in vec2 pos;
uniform vec2 mousePos;

layout(location=0) out vec4 outColor;
void main()
{
	float x = mousePos.x/640;
	float y = mousePos.y/480;
	vec2 mouseNormalised = vec2(2*x -1, 1 - 2*y);	//Don't know why I need to do this
	float d = distance(pos,mouseNormalised);
	outColor = vec4(d,d,d,1.0);
}
