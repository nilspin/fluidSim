in vec2 pos;
uniform vec2 mousePos;
uniform vec2 differenceLastPos;
uniform sampler2D velocity0;

layout(location=1) out vec4 velocity1;

void main()
{
	float X = mousePos.x/640;
	float Y = mousePos.y/480;
	vec2 mouseNormalised = vec2(X, 1 - Y);

	float dt = 1.0/60;
	float normalisedDifferenceX = differenceLastPos.x/640 ;
	float normalisedDifferenceY = differenceLastPos.y/480 ;

	vec2 displacement = vec2(normalisedDifferenceX,normalisedDifferenceY);

	vec2 mouseVel = displacement/dt;

	vec2 velocityIn = vec2(texture(velocity0, pos).xy);

	float dist = distance(pos,mouseNormalised);
	velocity1 = vec4(dist,dist,dist,1.0);
}