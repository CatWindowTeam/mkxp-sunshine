uniform sampler2D texture;
uniform sampler2D noiseTexture;

uniform float uTime;
uniform float aniIndex;

varying vec2 v_texCoord;
varying vec2 worldCoord;

uniform vec2 texSizeInv;

const float WATER_DISTORTION = 0.75;

const float atAreaW = 96.0;
const float atAreaH = 128.0*7.0;
const float SpriteAreaH = 128.0;
const float atAniOffset = 32.0*3.0;
const vec2 AtlasSize = vec2(96.0, 128.0);

float pnoise(vec2 P){
	vec2 uv = P;
	uv *= 0.025;
	uv -= floor(uv);
	return texture2D(noiseTexture, uv);
}

void main(){
	vec4 textureColor = texture2D(texture, v_texCoord);
	
	vec2 paletteUv = floor(v_texCoord / texSizeInv / AtlasSize) * AtlasSize;
	vec4 waterColor1 = texture2D(texture, (vec2(33.0, 1.0) + paletteUv) * texSizeInv) / 2.0;
	vec4 waterColor2 = texture2D(texture, (vec2(35.0, 1.0) + paletteUv) * texSizeInv) / 2.0;
	vec4 waterColor3 = texture2D(texture, (vec2(37.0, 1.0) + paletteUv) * texSizeInv) / 2.0;
	vec4 waterColorCheck = texture2D(texture, (vec2(33.0, 31.0) + paletteUv) * texSizeInv);

	vec2 uv = worldCoord / 16.0 * vec2(1.0, 4.0);
	uv = floor(uv * vec2(8.0, 2.0)) / vec2(8.0, 2.0);

	float noise = pnoise(uv);

	float offsetTime = uTime + 100.0;

	vec2 uv1 = uv - vec2(offsetTime, offsetTime * 0.5 + 0.5) * 0.56;
	float noise1 = pnoise(uv1 + vec2(WATER_DISTORTION, -WATER_DISTORTION) * noise);
	float p = pow(noise1 + 0.2, 10.0);

	vec2 uv2 = uv - vec2(offsetTime - 1.5, -offsetTime * 0.5) * 0.24;
	float noise2 = pnoise(uv2 + vec2(WATER_DISTORTION, -WATER_DISTORTION) * noise);
	float p2 = pow(noise2 + 0.2, 10.0);

	vec4 frag = mix(vec4(waterColor1.rgb, 1.0), vec4(waterColor2.rgb, 1.0), noise1) + vec4(vec3(mix(p, 1.0, textureColor.r)) * waterColor3.rgb, 1.0);
	    frag += mix(vec4(waterColor1.rgb, 1.0), vec4(waterColor2.rgb, 1.0), noise2) + vec4(vec3(mix(p2, 1.0, textureColor.r)) * waterColor3.rgb, 1.0);
	
	float isWater = float(
		waterColorCheck == vec4(1, 0, 0, 1)

	 && abs(textureColor.r - textureColor.g) < 0.02
	 && abs(textureColor.g - textureColor.b) < 0.02
	 
	 && textureColor.a > 0.05
	 && waterColor2.a > 0.05
	);

	gl_FragColor = mix(textureColor, frag, isWater);
}
