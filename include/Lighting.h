#pragma once
#include "LightingTemplate.h"
#include "Palette.h"
#include "Prop.h"

class Lighting : public Prop
{
public:
	Lighting(RE::TESObjectREFRPtr ref, int colorIdx, int lightTemplateIdx);

public:
	virtual void DrawControlPanel() override;
	void         UpdateLightColor();
	void         UpdateLightTemplate();
	void         Remove() override;

private:
	void ShadowSceneRemove(RE::ShadowSceneNode* shadowSceneNode);

private:
	RE::NiPointer<RE::BSLight>      bsLight = nullptr;
	RE::NiPointer<RE::NiPointLight> niLight = nullptr;
	Palette                         palette;
	LightingTemplate                lightTemplate;
};
