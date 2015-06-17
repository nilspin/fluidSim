#version 400

in vec3 Color;
in vec2 pos;
uniform vec2 mousePos;

layout(location=0) out vec4 outColor;
void main()
{
	float x = mousePos.x/640 ;
	float y = mousePos.y/480;
	vec2 mouseNormalised = vec2(x,1-y);//vec2(2*x -1, 1 - 2*y);	//Don't know why I need to do this
	
	float d = distance(gl_FragCoord.xy,vec2(mousePos.x,480 - mousePos.y));
//	outColor = vec4(d,d,d,1);
	if(d < 100){outColor = vec4(0,0,0,1.0);}
	else{outColor = vec4(1,1,1,1);}
	
	
}
