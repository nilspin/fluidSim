#version 400

in vec3 Color;
in vec2 pos;
uniform vec2 mousePos;
uniform vec2 iRes;

layout(location=0) out vec4 outColor;
void main()
{
	vec2 temp = mousePos*iRes;
	vec2 mouseNormalised = vec2(temp.x,1-temp.y);//vec2(2*x -1, 1 - 2*y);	//Don't know why I need to do this
	
	float d = distance(gl_FragCoord.xy,vec2(mousePos.x,480 - mousePos.y));
//	outColor = vec4(d,d,d,1);
	if(d < 100){outColor = vec4(0,0,0,1.0);}
	else{outColor = vec4(1,1,1,1);}
	
	
}
