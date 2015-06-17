#version 400

in vec2 pos;
uniform sampler2D pressure0;
uniform sampler2D divergence;
layout(location=2) out vec4 pressure1;

void main()
{
	float px = 1.0 / 640;
	float py = 1.0 / 480;
	float alpha = -1.0;
	float beta = 0.25;
	
	float x0 = texture(pressure0, pos - vec2(px, 0)).x;
	float x1 = texture(pressure0, pos + vec2(px, 0)).x;
	float y0 = texture(pressure0, pos - vec2(0, py)).x;
	float y1 = texture(pressure0, pos + vec2(0, py)).x;
	
	float d  = texture(divergence,pos).r;
	float relaxed = (x0 + x1 + y0 + y1 + alpha*d) * beta;
	
	pressure1 = vec4(relaxed);
}