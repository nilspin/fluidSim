in vec2 pos;
uniform vec2 mousePos;
layout(location=1) out vec4 velocity1;

void main()
{
	float x = mousePos.x/640;
	float y = mousePos.y/480;
	vec2 mouseNormalised = vec2(x, 1 - y);	//Because in OGL ordinates start from bottom, not up.
	float d = 1.0 - min(distance(pos,mouseNormalised),1.0);
	velocity1 = vec4(d);
}