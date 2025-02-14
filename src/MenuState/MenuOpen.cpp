#include "MenuState/MenuOpen.h"
#include "MenuState/MenuHidden.h"
#include "MenuState/MenuLookingAround.h"
#include "MenuState/MenuPositioning.h"
#include "MenuState/MenuItemActive.h"
#include "ImGui/ImGuiInputAdapter.h"

MenuOpen::MenuOpen(Input::MenuInputContext* inputCtx)
{
	inputCtx->MenuOpen();
	inputCtx->StopTextInput();
}

MenuStatePtr MenuOpen::Transition(Input::MenuInputContext* inputCtx)
{
	if (ImGui::ImGuiInputAdapter::IsKeyPressed("iHideKey", false))
	{
		return std::make_unique<MenuHidden>(inputCtx);
	}
	if (ImGui::ImGuiInputAdapter::IsKeyDown("iLookAroundKey"))
	{
		return std::make_unique<MenuLookingAround>(inputCtx);
	}
	if (ImGui::ImGuiInputAdapter::IsKeyDown("iPositionLightKey"))
	{
		return std::make_unique<MenuPositioning>(inputCtx);
	}
	if (ImGui::GetIO().WantTextInput)
	{
		return std::make_unique<MenuItemActive>(inputCtx);
	}
	return nullptr;
}

void MenuOpen::DoFrame(Chiaroscuro* menu)
{
	ImGui::Begin("##SCMain", nullptr, windowFlags);
	{
		DrawMenu(menu);
	}
	ImGui::End();
}

void MenuOpen::DrawMenu(Chiaroscuro* menu)
{
	menu->DrawTabBar();
	menu->DrawPropControlWindow();
	menu->DrawCameraControlWindow();
	menu->DrawSceneControlWindow();
}
