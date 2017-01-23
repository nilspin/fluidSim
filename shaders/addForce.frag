#version 330

in vec2 pos;
uniform vec2 mousePos;
uniform vec2 differenceLastPos;
uniform sampler2D velocity0;

layout(location=1) out vec4 velocity1;

void main()
{
	//DATA
	
	vec2 velocityIn = texture(velocity0,pos).xy;
	velocityIn *= 0.99;

	float x = mousePos.x/640 ;
	float y = mousePos.y/480;
	vec2 mouseNormalised = vec2(x,1-y);

	vec2 displacement = vec2(differenceLastPos.x/640 , differenceLastPos.y/480);

	float dt = 1.0/60;
	
	vec2 mouseVel = displacement/dt;

	float radius = 0.05;
	if(abs(distance(pos,mouseNormalised)) < radius)
	{
		vec2 velocityOut = 0.000001*velocityIn + 0.000001*mouseVel;
		velocity1 = vec4(velocityOut,0,1.0);
	}
	else
	{
		velocity1 = texture(velocity0,pos);
	}
}
