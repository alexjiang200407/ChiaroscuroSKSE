#include "MenuState/MenuItemActive.h"
#include "MenuState/MenuOpen.h"

MenuItemActive::MenuItemActive(Input::MenuInputContext* inputCtx)
{
	inputCtx->StartTextInput();
}

MenuStatePtr MenuItemActive::Transition(Input::MenuInputContext* inputCtx)
{
	if (!ImGui::GetIO().WantTextInput)
	{
		return std::make_unique<MenuOpen>(inputCtx);
	}
	return nullptr;
}
