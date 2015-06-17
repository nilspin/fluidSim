#version 400

in vec2 pos;
uniform sampler2D velocity0;
layout(location=4) out vec4 divergence;

void main()
{
	float px = 1.0 / 640;
	float py = 1.0 / 480;

//	float x0 = textureOffset(velocity0, pos, ivec2(-1, 0)).x;
//  float x1 = textureOffset(velocity0, pos, ivec2(1, 0)).x;
//  float y0 = textureOffset(velocity0, pos, ivec2(0, 1)).y;
//  float y1 = textureOffset(velocity0, pos, ivec2(0, -1)).y;
	
	float x0 = texture(velocity0,pos - vec2(px,0)).x;
	float x1 = texture(velocity0,pos + vec2(px,0)).x;
	float y0 = texture(velocity0,pos - vec2(0,py)).y;
	float y1 = texture(velocity0,pos + vec2(0,py)).y;

	float div = (x1-x0 + y1-y0)*0.5;
    divergence = vec4(div,0,0,1.0);//texture(velocity0, pos);//
}