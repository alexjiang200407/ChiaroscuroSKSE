#include "Lighting.h"

Lighting::Lighting(RE::TESObjectREFRPtr ref, preset::PresetDatabase* presetDB, preset::LightingPreset lightPreset) :
	Prop(ref), colorPalette(presetDB), lightCreateParams(lightPreset),
	fade(lightPreset.intensity), radius(lightPreset.radius, lightPreset.radius, lightPreset.radius)
{
}

Lighting::Lighting(RE::TESObjectREFRPtr ref, preset::Color color, preset::PresetDatabase* presetDB, preset::LightingPreset lightPreset) :
	Prop(ref), colorPalette(presetDB, color), lightCreateParams(lightPreset),
	fade(lightPreset.intensity), radius(lightPreset.radius, lightPreset.radius, lightPreset.radius)
{
}

void Lighting::DrawControlPanel()
{
	ImGui::BeginDisabled(!ref->Is3DLoaded() || !niLight || hideLight);
	{
		if (colorPalette.DrawEditor())
		{
			UpdateLightColor();
		}
		ImGui::SliderAutoFill("Light Radius", &niLight->radius.x, 32.0f, 1024.0f);
		ImGui::SliderAutoFill("Light Intensity", &niLight->fade, 0.0f, 10.0f);

		radius = niLight->radius;
		fade   = niLight->fade;
		DrawCameraOffsetSlider();
	}
	ImGui::EndDisabled();
	if (ImGui::ConditionalCheckbox("Hide Light", niLight.get(), &hideLight))
	{
		niLight->SetAppCulled(hideLight);
	}
	ImGui::SameLine();
	if (auto* model = ref->Get3D(); ImGui::ConditionalCheckbox("Hide Marker", model, &hideMarker))
	{
		model->GetObjectByName("Marker")->SetAppCulled(hideMarker);
	}
}

void Lighting::UpdateLightColor()
{
	if (auto selection = colorPalette.GetSelection())
		niLight->diffuse = *selection;
}

void Lighting::UpdateLightTemplate()
{
	auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];

	if (bsLight.get())
		shadowSceneNode->RemoveLight(bsLight);

	bsLight.reset(shadowSceneNode->AddLight(niLight.get(), lightCreateParams));
}

void Lighting::MoveToCameraLookingAt(bool resetOffset)
{
	assert(RE::PlayerCharacter::GetSingleton());
	assert(RE::PlayerCharacter::GetSingleton()->GetParentCell());

	if (GetCellID() != RE::PlayerCharacter::GetSingleton()->GetParentCell()->formID)
	{
		auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
		shadowSceneNode->RemoveLight(bsLight);
		shadowSceneNode->RemoveLight(niLight.get());
		Attach3D();
	}
	Prop::MoveToCameraLookingAt(resetOffset);
}

void Lighting::MoveTo(RE::NiPoint3 point)
{
	niLight->world.translate = point + offset;
	Prop::MoveTo(point);
}

void Lighting::OnEnterCell()
{
	// Skyrim creates a new niLight when we enter a cell
	// so we have to update our state
	Attach3D();
	UpdateLightColor();
	UpdateLightTemplate();
	MoveToCurrentPosition();
}

void Lighting::Remove()
{
	auto* shadowSceneNode                   = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
	shadowSceneNode->allowLightRemoveQueues = false;

	if (niLight.get())
		shadowSceneNode->RemoveLight(niLight.get());

	if (bsLight.get())
		shadowSceneNode->RemoveLight(bsLight);

	shadowSceneNode->allowLightRemoveQueues = true;
	
	niLight->SetAppCulled(true);
	Prop::Remove();
}

void Lighting::Rotate(RE::NiMatrix3 rotation)
{
	Prop::Rotate(rotation);
	if (niLight)
	{
		niLight->world.rotate = niLight->world.rotate * rotation;
	}
}

RE::BSFadeNode* Lighting::Attach3D()
{
	auto* niRoot = Prop::Attach3D();

	if (!niRoot)
		return nullptr;

	if (auto* newLight = niRoot->GetObjectByName("SceneCraftLight"))
	{
		auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
		shadowSceneNode->RemoveLight(niLight.get());
		niLight.reset(skyrim_cast<RE::NiPointLight*>(newLight));
	}
	else
	{
		auto* niNode = niRoot->GetObjectByName("AttachLight")->AsNode();
		niLight.reset(RE::NiPointLight::Create());
		niLight->name = "SceneCraftLight";
		RE::AttachNode(niNode, niLight.get());
		offset = niNode->local.translate;

		// TODO put these into a settings class
		niLight->ambient = RE::NiColor();
		niLight->radius  = radius;
		niLight->SetLightAttenuation(500);
		niLight->fade = fade;
	}

	return niRoot;
}

void Lighting::Init3D()
{
	Prop::Init3D();
	UpdateLightColor();
	UpdateLightTemplate();
}
