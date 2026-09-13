
uniform sampler2D texture;

uniform lowp vec4 tone;

uniform lowp float opacity;
uniform lowp vec4 color;
uniform lowp vec4 modulate;

uniform float bushDepth;
uniform lowp float bushOpacity;

uniform float uTime;

varying vec2 v_texCoord;

const vec3 lumaF = vec3(.299, .587, .114);

const bool simple = false;

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

float distanceFromOrigin(vec2 v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

void main(){
	// ------- crt --------
	vec2 uv = v_texCoord;

	float layers = 0.0;
	vec2 edge_dark = vec2(0);
	float edge_dark_res = 0.0;
	vec4 frag;

	uv -= 0.5;
	uv /= scale;

	float uv_dist = distanceFromOrigin(uv);
	uv = mix(uv, vec2(0), uv_dist * -disort) / (1.0 + disort / 2.0);

	float lines_uv = (uv.y / lines_scale) - uTime * lines_speed;
	float lines = floor(lines_uv + lines_fill) - floor(lines_uv);
	lines *= lines_opacity;
 
	if (!simple) {
		float layers_uv = (uv.y / layers_scale);
		layers = clamp(sin(layers_uv * 3.1415) - (1.0 - layers_fill), 0.0, 1.0);
		layers *= layers_opacity;

		edge_dark = abs(uv) * 2.0;
		edge_dark -= 1.0 - edge_dark_dist;
		edge_dark /= edge_dark_dist;
		edge_dark = clamp(edge_dark, 0.0, 1.0);
		edge_dark *= edge_dark_power;
		edge_dark_res = edge_dark.x + edge_dark.y;

		uv *= scale;
		uv += 0.5;

		vec4 frag1 = texture2D(texture, uv + vec2(-1, 1) * chromatic_power);
		vec4 frag2 = texture2D(texture, uv + vec2(1, -1) * chromatic_power);
		vec4 frag3 = texture2D(texture, uv);
		frag = vec4(frag1.r, frag2.g, frag3.b, (frag1.w + frag2.w + frag3.w) / 3.0);
	}
	else
	{
		uv *= scale;
		uv += 0.5;
		frag = texture2D(texture, uv);
	}
 
	frag += vec4(lines, lines, lines, 0.0) -
			vec4(layers, layers, layers, 0.0) -
			vec4(edge_dark_res, edge_dark_res, edge_dark_res, 0.0);

	frag.rgb = clamp(frag.rgb, 0.0, 1.0);

	// ------- original shit --------
	
	/* Apply gray */
	float luma = dot(frag.rgb, lumaF);
	frag.rgb = mix(frag.rgb, vec3(luma), tone.w);
	
	/* Apply tone */
	frag.rgb += tone.rgb;

	/* Apply opacity */
	frag.a *= opacity;
	
	/* Apply color */
	frag.rgb = mix(frag.rgb, color.rgb, color.a);

	/* Apply modulation */
	frag *= modulate;

	/* Apply bush alpha by mathematical if */
	lowp float underBush = float(v_texCoord.y < bushDepth);
	frag.a *= clamp(bushOpacity + underBush, 0.0, 1.0);
	
	gl_FragColor = mix(
		frag,
		vec4(0, 0, 0, 1),
		float(uv.x < 0.0) +
		float(uv.x > 1.0) +
		float(uv.y < 0.0) +
		float(uv.y > 1.0)
	);
}
