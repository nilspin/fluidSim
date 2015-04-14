in vec2 pos;
uniform sampler2D pressure0;
uniform sampler2D velocity1;
layout(location=0) out vec4 velocity0;

void main(){

	float px = 1.0 / 640;
	float py = 1.0 / 480;
	
    float x0 = texture2D(pressure0, pos - vec2(px, 0)).r;
    float x1 = texture2D(pressure0, pos+vec2(px, 0)).r;
    float y0 = texture2D(pressure0, pos-vec2(0, py)).r;
    float y1 = texture2D(pressure0, pos+vec2(0, py)).r;
    vec2 v = texture2D(velocity1, pos).xy;
    velocity0 = vec4((v-(vec2(x1, y1)-vec2(x0, y0))*0.5), 1.0, 1.0);
}