#pragma once
#include "render/irender.h"
#include <memory>
#include <vector>

template<typename T>
struct StateProperty{
	T current;
	std::vector<T> stack;

	void push() { stack.push_back(current); }
	void pop() { if (!stack.empty()) { current = stack.back(); stack.pop_back(); } }
	void set(const T &value) { current = value; }
	void pushSet(const T &value) { push(); set(value); }
};

template<typename T>
class HandlePool{
public:
	uint32_t add(std::unique_ptr<T> item){
		if (freeIds.empty()){
			items.push_back(std::move(item));
			return (uint32_t) items.size();
		}

		uint32_t id = freeIds.back();
		freeIds.pop_back();
		items[id-1] = std::move(item);
		return id;
	}

	T *get(uint32_t id) const{
		if (id == 0 || id > items.size())
			return 0;

		return items[id-1].get();
	}

	void remove(uint32_t id){
		items[id-1].reset();
		freeIds.push_back(id);
	}

	size_t size() const{
		return items.size();
	}

	T *at(size_t index) const{
		return items[index].get();
	}

private:
	std::vector<std::unique_ptr<T>> items;
	std::vector<uint32_t> freeIds;
};

enum class BlendKind{
	KeepDestAlpha,
	Normal,
	Addition,
	Substraction,
	Multiply,
	ToneAdd,
	ToneSubtract
};

struct EffectUniforms{
	float proj[16];
	float spriteMat[16];
	float matrix[16];
	Vec2i texSize;
	Vec2i translation;
	Vec2i offset;
	Vec4 tone;
	Vec4 color;
	Vec4 flash;
	Vec4 modulate;
	Vec4 subRect;
	float time;
	float opacity;
	float bushDepth;
	float bushOpacity;
	float gray;
	float hueAdjust;
	float aniIndex;
	float prog;
	float vague;
	uint32_t aux[4];

	EffectUniforms();
};

void multiplyMatrices(const float a[16], const float b[16], float out[16]);
void orthoMatrix(float out[16], int w, int h);
void perspectiveMatrix(float out[16], int w, int h, float fovDegrees);

class StatefulRender : public IRender{
public:
	explicit StatefulRender(const Config &conf);

	void bindTexture(TexHandle tex);
	void unbindTexture();

	void setViewport(const IntRect &rect);
	void pushViewport(const IntRect &rect);
	void popViewport();
	void refreshViewport();

	void pushBlend(bool enabled);
	void popBlend();

	void pushBlendMode(BlendType mode);
	void popBlendMode();
	void setBlendOverride(BlendOverride mode);
	void refreshBlendMode();

	void pushScissorTest(bool enabled);
	void popScissorTest();

	void pushScissorBox(const IntRect &rect);
	void saveScissorBox();
	void intersectScissorBox(const IntRect &rect);
	void popScissorBox();
	void setScissorBox(const IntRect &rect);
	const IntRect &scissorBox();

	void pushClearColor(const Vec4 &color);
	void popClearColor();

	void useEffect(ShaderType effect);
	void useBlurPass(int pass);
	void applyViewportProj();
	void applyPerspectiveProj();
	void setTexSize(const Vec2i &size);
	void setTranslation(const Vec2i &value);
	void setTime(float value);
	void setEffectTexture(EffectTexture slot, TexHandle tex);
	void setSpriteMat(const float value[16]);
	void setMatrix(const float value[16]);
	void setTone(const Vec4 &value);
	void setColor(const Vec4 &value);
	void setFlash(const Vec4 &value);
	void setModulate(const Vec4 &value);
	void setOpacity(float value);
	void setBushDepth(float value);
	void setBushOpacity(float value);
	void setGray(float value);
	void setHueAdjust(float value);
	void setAniIndex(int value);
	void setOffset(const Vec2i &value);
	void setSubRect(const FloatRect &value);
	void setProg(float value);
	void setVague(float value);
	void setWallMapResolution(int x, int y);
	void setCameraPosition(int x, int y);
	void setTileMapOffset(int x, int y);
	void setLightSources(const std::vector<LightSource> &sources);
	void setAmbient(float value);

protected:
	EffectUniforms &uniforms();
	void resetBlend();

	StateProperty<IntRect> viewport;
	StateProperty<bool> blend;
	StateProperty<BlendType> blendModeProp;
	StateProperty<bool> scissorTest;
	StateProperty<IntRect> scissorBoxProp;
	StateProperty<Vec4> clearColor;
	BlendKind activeBlend;

	uint32_t boundTexture;
	std::vector<EffectUniforms> uniformSets;
	int currentSlot;
	int blurPass;

	std::vector<LightSource> lights;
	Vec2i cameraPos;
	float ambient;
};
