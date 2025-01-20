#pragma once
#include "ImGuiRenderTarget.h"
#include <fmt/compile.h>

namespace ImGui
{
	class ImGuiRenderer
	{
	private:
		struct WndProc
		{
			static LRESULT        thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			static inline WNDPROC func;
		};

		struct CreateD3DAndSwapChain
		{
			static void                                    thunk();
			static inline REL::Relocation<decltype(thunk)> func;
		};

		struct StopTimer
		{
			static void                                    thunk(std::uint32_t timer);
			static inline REL::Relocation<decltype(thunk)> func;
		};

	private:
		ImGuiRenderer()                                = default;
		ImGuiRenderer(const ImGuiRenderer&)            = delete;
		ImGuiRenderer& operator=(const ImGuiRenderer&) = delete;
		ImGuiRenderer(ImGuiRenderer&&)                 = delete;
		ImGuiRenderer& operator=(ImGuiRenderer&&)      = delete;

	public:
		void                  Init(ImGuiStyle a_style);
		void                  RegisterRenderTarget(ImGuiRenderTarget* target);
		void                  UnregisterRenderTarget(ImGuiRenderTarget* target);
		static ImGuiRenderer* GetSingleton();

	private:
		ImGuiStyle                   style;
		std::set<ImGuiRenderTarget*> targets;
		std::atomic<bool>            installedHooks;
		std::string                  iniFile = fmt::format("./Data/SKSE/Plugins/{}ImGui.ini", Version::PROJECT);
		static ImGuiRenderer         singleton;
	};
}
