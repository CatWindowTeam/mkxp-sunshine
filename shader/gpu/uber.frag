#version 450

layout(location = 0) in vec2 vTex;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec2 vWorld;
layout(location = 3) in vec4 vBlur;

layout(set = 2, binding = 0) uniform sampler2D tex0;
layout(set = 2, binding = 1) uniform sampler2D tex1;
layout(set = 2, binding = 2) uniform sampler2D tex2;
layout(set = 2, binding = 3) uniform sampler2D tex3;

layout(set = 3, binding = 0) uniform FragParams{
	vec4 tone;
	vec4 color;
	vec4 flash;
	vec4 modulate;
	vec4 subRect;
	vec2 texSizeInv;
	float opacity;
	float gray;
	float hueAdjust;
	float bushDepth;
	float bushOpacity;
	float uTime;
	float prog;
	float vague;
	int effect;
	float pad0;
} f;

layout(location = 0) out vec4 outColor;

const vec3 lumaF = vec3(.299, .587, .114);
const float WATER_DISTORTION = 0.75;

const int FX_FLAT = 0;
const int FX_SIMPLE = 1;
const int FX_SIMPLE_COLOR = 2;
const int FX_SIMPLE_ALPHA = 3;
const int FX_SIMPLE_SPRITE = 4;
const int FX_ALPHA_SPRITE = 5;
const int FX_SPRITE = 6;
const int FX_WORLD_MACHINE = 7;
const int FX_WATER = 8;
const int FX_CRT = 9;
const int FX_TILEMAP_WATER = 10;
const int FX_PLANE = 11;
const int FX_GRAY = 12;
const int FX_TILEMAP = 13;
const int FX_FLASH = 14;
const int FX_TRANS = 15;
const int FX_SIMPLE_TRANS = 16;
const int FX_HUE = 17;
const int FX_BLT = 18;
const int FX_SIMPLE_MATRIX = 19;
const int FX_BLUR = 20;
const int FX_OBSCURED = 21;

float pnoise(vec2 P){
	vec2 uv = P;
	uv *= 0.025;
	uv -= floor(uv);
	return texture(tex1, uv).x;
}

vec4 spriteCommon(vec4 frag, bool modulateEarly){
	float luma = dot(frag.rgb, lumaF);
	frag.rgb = mix(frag.rgb, vec3(luma), f.tone.w);

	if (modulateEarly)
		frag *= f.modulate;

	frag.rgb += f.tone.rgb;
	frag.a *= f.opacity;
	frag.rgb = mix(frag.rgb, f.color.rgb, f.color.a);

	if (!modulateEarly)
		frag *= f.modulate;

	float underBush = float(vTex.y < f.bushDepth);
	frag.a *= clamp(f.bushOpacity + underBush, 0.0, 1.0);
	return frag;
}

vec3 rgb2hsv(vec3 c){
	const vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
	vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
	vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
	float d = q.x - min(q.w, q.y);
	const float eps = 1.0e-10;
	return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + eps)), d / (q.x + eps), q.x);
}

vec3 hsv2rgb(vec3 c){
	const vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec2 rand(float seed){
	return fract(sin(vec2(seed * 12.9898, seed * 78.233)) * 43758.5453);
}

vec4 clampedTexture(vec2 uv){
	if (uv.x > 1.0 || uv.y > 1.0 || uv.x < 0.0 || uv.y < 0.0)
		return vec4(0, 0, 0, 0);

	return texture(tex0, uv);
}

vec4 worldMachine(){
	vec4 frag = texture(tex0, vTex);

	float luma = dot(frag.rgb, lumaF);
	frag.rgb = mix(frag.rgb, vec3(luma), f.tone.w);
	frag.rgb += f.tone.rgb;
	frag.rgb = mix(frag.rgb, f.color.rgb, f.color.a);
	frag *= f.modulate;

	float randTime = rand(floor(f.uTime * 15.0)).x;
	vec4 g1 = clampedTexture(vTex + floor(randTime * 2.0 - 0.5) * f.texSizeInv);
	vec4 g2 = clampedTexture(vTex + floor(rand(floor(f.uTime * 15.0 + 67.6767)) * 2.0 - 0.5) * f.texSizeInv);
	vec4 g3 = clampedTexture(vTex + floor(rand(floor(f.uTime * 15.0 + 42.5252)) * 2.0 - 0.5) * f.texSizeInv);
	vec3 glitch = vec3(g1.r, g2.g * floor(randTime + 0.5), g3.b * floor(1.0 - randTime + 0.5));
	frag.rgb += mix(glitch, glitch * 0.25, frag.a);
	frag.a += (g1.a + g2.a + g3.a) * 0.125;

	frag.a *= f.opacity;

	float underBush = float(vTex.y < f.bushDepth);
	frag.a *= clamp(f.bushOpacity + underBush, 0.0, 1.0);
	frag.a = clamp(frag.a, 0.0, 1.0);
	return frag;
}

vec4 crt(){
	const float disort = 0.5;
	const float lines_scale = 3.0;
	const float lines_fill = 0.075;
	const float lines_speed = 0.19;
	const float lines_opacity = 0.25;
	const float layers_scale = 0.01;
	const float layers_fill = 0.5;
	const float layers_opacity = 0.25;
	const float edge_dark_power = 1.0;
	const float edge_dark_dist = 0.05;
	const float chromatic_power = 0.0005;
	const vec2 scale = vec2(0.85, 0.8);

	vec2 uv = vTex;
	uv -= 0.5;
	uv /= scale;

	float uv_dist = length(uv);
	uv = mix(uv, vec2(0), uv_dist * -disort) / (1.0 + disort / 2.0);

	float lines_uv = (uv.y / lines_scale) - f.uTime * lines_speed;
	float lines = floor(lines_uv + lines_fill) - floor(lines_uv);
	lines *= lines_opacity;

	float layers_uv = (uv.y / layers_scale);
	float layers = clamp(sin(layers_uv * 3.1415) - (1.0 - layers_fill), 0.0, 1.0);
	layers *= layers_opacity;

	vec2 edge_dark = abs(uv) * 2.0;
	edge_dark -= 1.0 - edge_dark_dist;
	edge_dark /= edge_dark_dist;
	edge_dark = clamp(edge_dark, 0.0, 1.0);
	edge_dark *= edge_dark_power;
	float edge_dark_res = edge_dark.x + edge_dark.y;

	uv *= scale;
	uv += 0.5;

	vec4 frag1 = texture(tex0, uv + vec2(-1, 1) * chromatic_power);
	vec4 frag2 = texture(tex0, uv + vec2(1, -1) * chromatic_power);
	vec4 frag3 = texture(tex0, uv);
	vec4 frag = vec4(frag1.r, frag2.g, frag3.b, (frag1.w + frag2.w + frag3.w) / 3.0);

	frag += vec4(lines, lines, lines, 0.0) - vec4(layers, layers, layers, 0.0) - vec4(edge_dark_res, edge_dark_res, edge_dark_res, 0.0);
	frag.rgb = clamp(frag.rgb, 0.0, 1.0);

	frag = spriteCommon(frag, false);

	return mix(frag, vec4(0, 0, 0, 1), float(uv.x < 0.0) + float(uv.x > 1.0) + float(uv.y < 0.0) + float(uv.y > 1.0));
}

vec4 water(){
	vec2 uv = vTex * f.texSizeInv * 8192.0;
	uv = floor(uv * 16.0) / 16.0;

	float noise = pnoise(uv);
	vec2 uv1 = uv - vec2(f.uTime, f.uTime * 0.5 + 0.5) * 0.56;
	float noise1 = pnoise(uv1 + vec2(WATER_DISTORTION, -WATER_DISTORTION) * noise);
	float p = pow(noise1 + 0.02, 20.0);

	vec2 uv2 = uv - vec2(f.uTime - 1.5, -f.uTime * 0.5) * 0.24;
	float noise2 = pnoise(uv2 + vec2(WATER_DISTORTION, -WATER_DISTORTION) * noise);
	float p2 = pow(noise2 + 0.02, 20.0);

	vec4 frag = mix(vec4(f.color.r, f.color.g, f.color.b, 1.0) * f.color.a, vec4(f.color.r, f.color.g, f.color.b, 1.0) * f.tone.a, noise1) + vec4(p, p, p, 1.0) * f.tone;
	frag += mix(vec4(f.color.r, f.color.g, f.color.b, 1.0) * f.color.a, vec4(f.color.r, f.color.g, f.color.b, 1.0) * f.tone.a, noise2) + vec4(p2, p2, p2, 1.0) * f.tone;
	frag.a = clamp(frag.a / 2.0, 0.0, 1.0);
	frag.rgb = clamp(frag.rgb, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));

	frag.a *= f.opacity;
	frag *= f.modulate;
	return frag;
}

vec4 tilemapWater(){
	const vec2 AtlasSize = vec2(96.0, 128.0);

	vec4 textureColor = texture(tex0, vTex);

	vec2 paletteUv = floor(vTex / f.texSizeInv / AtlasSize) * AtlasSize;
	vec4 waterColor1 = texture(tex0, (vec2(33.0, 1.0) + paletteUv) * f.texSizeInv) / 2.0;
	vec4 waterColor2 = texture(tex0, (vec2(35.0, 1.0) + paletteUv) * f.texSizeInv) / 2.0;
	vec4 waterColor3 = texture(tex0, (vec2(37.0, 1.0) + paletteUv) * f.texSizeInv) / 2.0;
	vec4 waterColorCheck = texture(tex0, (vec2(33.0, 31.0) + paletteUv) * f.texSizeInv);

	vec2 uv = vWorld / 16.0 * vec2(1.0, 4.0);
	uv = floor(uv * vec2(8.0, 2.0)) / vec2(8.0, 2.0);

	float noise = pnoise(uv);
	float offsetTime = f.uTime + 100.0;

	vec2 uv1 = uv - vec2(offsetTime, offsetTime * 0.5 + 0.5) * 0.56;
	float noise1 = pnoise(uv1 + vec2(WATER_DISTORTION, -WATER_DISTORTION) * noise);
	float p = pow(noise1 + 0.2, 10.0);

	vec2 uv2 = uv - vec2(offsetTime - 1.5, -offsetTime * 0.5) * 0.24;
	float noise2 = pnoise(uv2 + vec2(WATER_DISTORTION, -WATER_DISTORTION) * noise);
	float p2 = pow(noise2 + 0.2, 10.0);

	vec4 frag = mix(vec4(waterColor1.rgb, 1.0), vec4(waterColor2.rgb, 1.0), noise1) + vec4(vec3(mix(p, 1.0, textureColor.r)) * waterColor3.rgb, 0.0);
	frag += mix(vec4(waterColor1.rgb, 1.0), vec4(waterColor2.rgb, 1.0), noise2) + vec4(vec3(mix(p2, 1.0, textureColor.r)) * waterColor3.rgb, 0.0);
	frag.a = clamp(frag.a / 2.0, 0.0, 1.0);
	frag.rgb = clamp(frag.rgb, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));

	float isWater = float(
		waterColorCheck == vec4(1, 0, 0, 1)
	 && abs(textureColor.r - textureColor.g) < 0.02
	 && abs(textureColor.g - textureColor.b) < 0.02
	 && textureColor.a > 0.05
	 && waterColor2.a > 0.05
	);

	return mix(textureColor, frag, isWater);
}

void main(){
	vec4 frag;

	switch (f.effect){
	case FX_FLAT :
		frag = f.color;
		break;

	case FX_SIMPLE :
	case FX_SIMPLE_SPRITE :
	case FX_TILEMAP :
		frag = texture(tex0, vTex);
		break;

	case FX_SIMPLE_COLOR :
		frag = vColor;
		break;

	case FX_SIMPLE_ALPHA :
	case FX_SIMPLE_MATRIX :
		frag = texture(tex0, vTex);
		frag.a *= vColor.a;
		break;

	case FX_ALPHA_SPRITE :
		frag = texture(tex0, vTex);
		frag.a *= f.opacity;
		break;

	case FX_SPRITE :
		frag = spriteCommon(texture(tex0, vTex), true);
		break;

	case FX_WORLD_MACHINE :
		frag = worldMachine();
		break;

	case FX_CRT :
		frag = crt();
		break;

	case FX_WATER :
		frag = water();
		break;

	case FX_TILEMAP_WATER :
		frag = tilemapWater();
		break;

	case FX_PLANE :
		{
			frag = texture(tex0, vTex);
			float luma = dot(frag.rgb, lumaF);
			frag.rgb = mix(frag.rgb, vec3(luma), f.tone.w);
			frag.rgb += f.tone.rgb;
			frag.a *= f.opacity;
			frag.rgb = mix(frag.rgb, f.color.rgb, f.color.a);
			frag.rgb = mix(frag.rgb, f.flash.rgb, f.flash.a);
			break;
		}

	case FX_GRAY :
		{
			frag = texture(tex0, vTex);
			float luma = dot(frag.rgb, lumaF);
			frag.rgb = mix(frag.rgb, vec3(luma), f.gray);
			break;
		}

	case FX_FLASH :
		frag = vec4(vColor.rgb * f.opacity, 1.0);
		break;

	case FX_TRANS :
		{
			float transV = 1.0 - (1.0 - texture(tex3, vTex).r) * (1.0 - f.vague);
			float cTransV = clamp(transV, f.prog, f.prog + f.vague);
			float alpha = (cTransV - f.prog) / f.vague;
			frag = mix(texture(tex1, vTex), texture(tex2, vTex), alpha);
			break;
		}

	case FX_SIMPLE_TRANS :
		frag = mix(texture(tex2, vTex), texture(tex1, vTex), f.prog);
		break;

	case FX_HUE :
		{
			frag = texture(tex0, vTex);
			vec3 hsv = rgb2hsv(frag.rgb);
			hsv.x += f.hueAdjust;
			frag.rgb = hsv2rgb(hsv);
			break;
		}

	case FX_BLT :
		{
			vec2 dstCoor = (vTex - f.subRect.xy) * f.subRect.zw;
			vec4 srcFrag = texture(tex0, vTex);
			vec4 dstFrag = texture(tex1, dstCoor);

			float co1 = srcFrag.a * f.opacity;
			float co2 = dstFrag.a * (1.0 - co1);
			frag.a = co1 + co2;

			if (frag.a == 0.0)
				frag.rgb = srcFrag.rgb;
			else
				frag.rgb = (co1 * srcFrag.rgb + co2 * dstFrag.rgb) / frag.a;
			break;
		}

	case FX_BLUR :
		frag = (texture(tex0, vTex) + texture(tex0, vBlur.xy) + texture(tex0, vBlur.zw)) / 3.0;
		break;

	case FX_OBSCURED :
		frag = texture(tex0, vTex);
		frag.a *= texture(tex1, vTex).r;
		break;

	default :
		frag = vec4(1.0, 0.0, 1.0, 1.0);
		break;
	}

	outColor = frag;
}
