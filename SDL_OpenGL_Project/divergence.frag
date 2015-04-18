in vec2 pos;
uniform sampler2D velocity1;
layout(location=4) out float divergence;

void main()
{
	float px = 1.0 / 640;
	float py = 1.0 / 480;

	float x0 = texture2D(velocity1, pos - vec2(px, 0)).x;
    float x1 = texture2D(velocity1, pos - vec2(px, 0)).x;
    float y0 = texture2D(velocity1, pos - vec2(0, py)).y;
    float y1 = texture2D(velocity1, pos - vec2(0, py)).y;
	
    divergence = (x1-x0 + y1-y0)*0.5;
}