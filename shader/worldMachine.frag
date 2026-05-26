uniform sampler2D texture;
uniform vec2 texSizeInv;

uniform lowp vec4 tone;

uniform lowp float opacity;
uniform lowp vec4 color;
uniform lowp vec4 modulate;

uniform float bushDepth;
uniform lowp float bushOpacity;

uniform float uTime;

varying vec2 v_texCoord;

const vec3 lumaF = vec3(.299, .587, .114);

vec2 rand(float seed) {
	return fract(sin(vec2(seed * 12.9898, seed * 78.233)) * 43758.5453);
}

vec4 clampedTexture2D(sampler2D s, vec2 uv){
	if (uv.x > 1.0 || uv.y > 1.0 || uv.x < 0.0 || uv.y < 0.0)
		return vec4(0, 0, 0, 0);
	else
		return texture2D(s, uv);
}

void main(){

	/* Sample source color */
	vec4 frag = texture2D(texture, v_texCoord);
	
	/* Apply gray */
	float luma = dot(frag.rgb, lumaF);
	frag.rgb = mix(frag.rgb, vec3(luma), tone.w);
	
	/* Apply tone */
	frag.rgb += tone.rgb;
	
	/* Apply color */
	frag.rgb = mix(frag.rgb, color.rgb, color.a);

	/* Apply modulation */
	frag *= modulate;

	/* glitch effect */
	float randTime = rand(floor(uTime * 15.0)).x;
	vec4 glitchFrag1 = clampedTexture2D(texture, v_texCoord + floor(randTime * 2.0 - 0.5) * texSizeInv);
	vec4 glitchFrag2 = clampedTexture2D(texture, v_texCoord + floor(rand(floor(uTime * 15.0 + 67.6767)) * 2.0 - 0.5) * texSizeInv);
	vec4 glitchFrag3 = clampedTexture2D(texture, v_texCoord + floor(rand(floor(uTime * 15.0 + 42.5252)) * 2.0 - 0.5) * texSizeInv);
	vec3 glitchFrag = vec3(glitchFrag1.r, glitchFrag2.g * floor(randTime + 0.5), glitchFrag3.b * floor(1.0 - randTime + 0.5));
	frag.rgb += mix(glitchFrag, glitchFrag * 0.25, frag.a);
	frag.a += (glitchFrag1.a + glitchFrag2.a + glitchFrag3.a) * 0.125;

	/* Apply opacity */
	frag.a *= opacity;

	/* Apply bush alpha by mathematical if */
	lowp float underBush = float(v_texCoord.y < bushDepth);
	frag.a *= clamp(bushOpacity + underBush, 0.0, 1.0);

	gl_FragColor = frag;
}
