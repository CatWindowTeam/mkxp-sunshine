uniform sampler2D texture;
uniform sampler2D noiseTexture;

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

float pnoise(vec2 P){
	vec2 uv = P;
	uv *= 0.025;
	uv -= floor(uv);
	return texture2D(noiseTexture, uv).x;
}

void main(){
	vec2 uv = v_texCoord * texSizeInv * 8192.0;
	uv = floor(uv * 16.0) / 16.0;

	float noise = pnoise(uv);
	vec2 uv1 = uv - vec2(uTime, uTime * 0.5 + 0.5) * 0.56;
	float noise1 = pnoise(uv1 + vec2(WATER_DISTORTION, -WATER_DISTORTION) * noise);
	float p = pow(noise1 + 0.02, 20.0);

	vec2 uv2 = uv - vec2(uTime - 1.5, -uTime * 0.5) * 0.24;
	float noise2 = pnoise(uv2 + vec2(WATER_DISTORTION, -WATER_DISTORTION) * noise);
	float p2 = pow(noise2 + 0.02, 20.0);

	vec4 frag = mix(vec4(color.r, color.g, color.b, 1.0) * color.a, vec4(color.r, color.g, color.b, 1.0) * tone.a, noise1) + vec4(p, p, p, 1.0) * tone;
		frag += mix(vec4(color.r, color.g, color.b, 1.0) * color.a, vec4(color.r, color.g, color.b, 1.0) * tone.a, noise2) + vec4(p2, p2, p2, 1.0) * tone;
		frag.a = clamp(frag.a / 2.0, 0.0, 1.0);
		frag.rgb = clamp(frag.rgb, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));

	/* Apply opacity */
	frag.a *= opacity;

	/* Apply modulation */
	frag *= modulate;
	
	gl_FragColor = frag;
}
