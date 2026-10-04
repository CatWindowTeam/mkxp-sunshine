#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTex;
layout(location = 2) in vec4 inColor;

layout(set = 1, binding = 0) uniform VertexParams{
	mat4 xform;
	vec2 translation;
	vec2 texSizeInv;
	vec2 offset;
	float aniIndex;
	int tilemapMode;
	int blurMode;
	int pad0;
	int pad1;
	int pad2;
} u;

layout(location = 0) out vec2 vTex;
layout(location = 1) out vec4 vColor;
layout(location = 2) out vec2 vWorld;
layout(location = 3) out vec4 vBlur;

void main(){
	vec2 tex = inTex;

	if (u.tilemapMode != 0){
		float pred = float(tex.x <= 96.0 && tex.y <= 896.0);
		tex.x += u.aniIndex * 96.0 * pred;
	}

	vec4 p = u.xform * vec4(inPos + u.translation, 0.0, 1.0);
	gl_Position = vec4(p.x, -p.y, 0.0, p.w);

	vTex = tex * u.texSizeInv;
	vColor = inColor;
	vWorld = inPos + u.offset * 32.0;

	vBlur = vec4(0.0);
	if (u.blurMode == 1)
		vBlur = vec4(vec2(tex.x - 1.0, tex.y) * u.texSizeInv, vec2(tex.x + 1.0, tex.y) * u.texSizeInv);
	else if (u.blurMode == 2)
		vBlur = vec4(vec2(tex.x, tex.y - 1.0) * u.texSizeInv, vec2(tex.x, tex.y + 1.0) * u.texSizeInv);
}
