#pragma once

class Prop
{
public:
	Prop(RE::TESObjectREFRPtr ref);

public:
	virtual bool DrawTabItem(bool& active);
	virtual void DrawControlPanel();
	virtual void Remove();
	virtual void MoveToCameraLookingAt(float distanceFromCamera);
	virtual void Hide();

private:
	static RE::NiPoint3 GetCameraPosition();

protected:
	RE::TESObjectREFRPtr ref;
};
