uniform sampler2D texture;

uniform lowp vec4 tone;

uniform lowp float opacity;
uniform lowp vec4 color;
uniform lowp vec4 modulate;

uniform float bushDepth;
uniform lowp float bushOpacity;

uniform float uTime;

varying vec2 v_texCoord;

uniform vec2 texSizeInv;

const vec3 lumaF = vec3(.299, .587, .114);
const float WATER_DISTORTION = 0.75;

vec4 mod289(vec4 x){
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x){
  return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r){
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec2 fade(vec2 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

float pnoise(vec2 P){
	vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
	vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
	Pi = mod289(Pi);        // To avoid truncation effects in permutation
	vec4 ix = Pi.xzxz;
	vec4 iy = Pi.yyww;
	vec4 fx = Pf.xzxz;
	vec4 fy = Pf.yyww;

	vec4 i = permute(permute(ix) + iy);

	vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
	vec4 gy = abs(gx) - 0.5 ;
	vec4 tx = floor(gx + 0.5);
	gx = gx - tx;

	vec2 g00 = vec2(gx.x,gy.x);
	vec2 g10 = vec2(gx.y,gy.y);
	vec2 g01 = vec2(gx.z,gy.z);
	vec2 g11 = vec2(gx.w,gy.w);

	vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
	g00 *= norm.x;
	g01 *= norm.y;
	g10 *= norm.z;
	g11 *= norm.w;

	float n00 = dot(g00, vec2(fx.x, fy.x));
	float n10 = dot(g10, vec2(fx.y, fy.y));
	float n01 = dot(g01, vec2(fx.z, fy.z));
	float n11 = dot(g11, vec2(fx.w, fy.w));

	vec2 fade_xy = fade(Pf.xy);
	vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
	float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
	return 2.3 * n_xy;
}

void main(){
	vec2 uv = v_texCoord * texSizeInv * 8192.0;
	uv = floor(uv * 16.0) / 16.0;

	float noise = (pnoise(uv) + 1.0) / 2.0;
	vec2 uv1 = uv - vec2(uTime, uTime * 0.5 + 0.5) * 0.56;
	float noise1 = (pnoise(uv1 + vec2(WATER_DISTORTION, -WATER_DISTORTION) * noise) + 1.0) / 2.0;
	float p = pow(noise1 + 0.02, 20.0);

	vec2 uv2 = uv - vec2(uTime - 1.5, -uTime * 0.5) * 0.24;
	float noise2 = (pnoise(uv2 + vec2(WATER_DISTORTION, -WATER_DISTORTION) * noise) + 1.0) / 2.0;
	float p2 = pow(noise2 + 0.02, 20.0);

	vec4 frag = mix(vec4(color.r, color.g, color.b, 1.0) * color.a, vec4(color.r, color.g, color.b, 1.0) * tone.a, noise1) + vec4(p, p, p, 1.0) * tone;
	frag += mix(vec4(color.r, color.g, color.b, 1.0) * color.a, vec4(color.r, color.g, color.b, 1.0) * tone.a, noise2) + vec4(p2, p2, p2, 1.0) * tone;

	/* Apply opacity */
	frag.a *= opacity;

	/* Apply modulation */
	frag *= modulate;
	
	gl_FragColor = frag;
}
