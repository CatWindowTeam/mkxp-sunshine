#version 450

layout(location = 0) in vec2 vTex;

layout(set = 3, binding = 0) uniform LightParams{
	vec4 lightSources[64];
	vec4 lightSourcesColors[64];
	vec2 texSizeInv;
	vec2 cameraPosition;
	float ambientLight;
	int lightSourcesCount;
	float pad0;
	float pad1;
} l;

layout(location = 0) out vec4 outColor;

const vec2 tileSize = vec2(32, 32);
const float powerBase = 255.0;

void main(){
	vec2 screenPoint = vTex / l.texSizeInv;
	vec3 light = vec3(0, 0, 0);

	for (int i = 0; i < l.lightSourcesCount; i++){
		vec4 sourceInfo = l.lightSources[i];
		vec4 sourceColor = l.lightSourcesColors[i];
		vec2 lightScreenPoint = (sourceInfo.xy - l.cameraPosition / tileSize) * tileSize + tileSize / 2.0;
		light += sourceInfo.z / powerBase *
		         sourceColor.rgb *
		         pow(clamp(1.0 - distance(screenPoint, lightScreenPoint) / 32.0 / sourceInfo.w, 0.0, 1.0), sourceColor.a * 2.0);
	}

	light += l.ambientLight / powerBase;

	outColor = vec4(light, 1.0);
}
