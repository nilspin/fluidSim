in vec2 pos;
uniform vec2 mousePos;
layout(location=0) out vec4 velocity1;

void main()
{
	float x = mousePos.x/640;
	float y = mousePos.y/480;
	vec2 mouseNormalised = vec2(2*x -1, 1 - 2*y);	//Don't know why I need to do this
	float d = distance(pos,mouseNormalised);
	velocity1 = vec4(d,d,d,1.0);
}