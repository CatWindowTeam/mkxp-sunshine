uniform vec4 lightSources[64];
uniform vec4 lightSourcesColors[64];
uniform int lightSourcesCount;
uniform float ambientLight;

uniform sampler2D texture;
//uniform sampler2D wallMapTexture;

uniform vec2 wallMapResolution;

uniform vec2 cameraPosition;
uniform vec2 tileMapOffset;

uniform vec2 texSizeInv;
varying vec2 v_texCoord;

const vec2 tileSize = vec2(32, 32);
const float powerBase = 255.0;

void main(){
	vec2 screenPoint = v_texCoord / texSizeInv;
	//vec2 mapPoint = cameraPosition + screenPoint;
	//vec2 wallmapUV = mapPoint / tileSize / wallMapResolution;

	vec3 light = vec3(0, 0, 0);

	for(int i = 0; i < lightSourcesCount; i++) {
		vec4 sourceInfo = lightSources[i];
		vec4 sourceColor = lightSourcesColors[i];
		vec2 lightScreenPoint = (sourceInfo.xy - cameraPosition / tileSize) * tileSize + tileSize / 2.0;
		light += sourceInfo.z / powerBase *
				 sourceColor.rgb *
				 pow(clamp(1.0 - distance(screenPoint, lightScreenPoint) / 32.0 / sourceInfo.w, 0.0, 1.0), sourceColor.a * 2.0);
	}

	light += ambientLight / powerBase;

	gl_FragColor = vec4(light, 1.0);
}
