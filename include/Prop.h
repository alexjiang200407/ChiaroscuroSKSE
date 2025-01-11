#pragma once
#include "ImGui/ImGuiTabBar.h"
#include "Preset/PresetDatabase.h"
#include "ModelPreset.h"
#include "ImGui/ImGuiPresetSelector.h"

class Prop :
	public ImGui::ImGuiTabBarItemRemovable
{
public:
	typedef ImGui::ImGuiPresetSelector<preset::ModelPreset> ModelSelector;

public:
	Prop(RE::TESObjectREFRPtr ref, preset::PresetDatabase* presetDB);

public:
	virtual bool            DrawTabItem(bool& active) override;
	virtual void            DrawControlPanel();
	virtual void            Remove() override;
	virtual void            MoveToCameraLookingAt(float distanceFromCamera);
	virtual void            MoveTo(RE::NiPoint3 newPos);
	virtual void            Hide();
	bool                    isHidden() const;
	void                    MoveToCurrentPosition();
	virtual void            Show();
	void                    Switch3D(preset::ModelPreset* preset);
	void                    Switch3D(RE::TESBoundObject* modelBoundObj);
	virtual void            OnEnterCell();
	RE::FormID              GetCellID();
	void                    Rotate(float delta);
	virtual void            Rotate(RE::NiMatrix3 rotation);
	static RE::NiPoint3     GetCameraLookingAt(float distanceFromCamera);
	virtual RE::BSFadeNode* Attach3D();
	virtual void            Init3D();

private:
	static RE::NiPoint3 GetCameraPosition();

protected:
	RE::TESObjectREFRPtr ref;

private:
	ModelSelector             modelSelector;
	RE::NiPointer<RE::NiNode> prop3D;
	bool                      hidden = false;
	RE::NiPoint3              worldTranslate;
};
