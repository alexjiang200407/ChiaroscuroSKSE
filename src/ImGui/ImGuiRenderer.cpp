#include "ImGui/ImGuiRenderer.h"
#include <dxgi.h>

void ImGui::ImGuiRenderer::CreateD3DAndSwapChain::thunk()
{
	func();
	const auto renderer = RE::BSGraphics::Renderer::GetSingleton();

	if (!renderer)
	{
		return;
	}

	const auto swapChain = (IDXGISwapChain*)renderer->data.renderWindows[0].swapChain;
	if (!swapChain)
	{
		logger::error("couldn't find swapChain");
		return;
	}
	DXGI_SWAP_CHAIN_DESC desc{};
	if (FAILED(swapChain->GetDesc(std::addressof(desc))))
	{
		logger::error("IDXGISwapChain::GetDesc failed.");
		return;
	}

	const auto device  = (ID3D11Device*)renderer->data.forwarder;
	const auto context = (ID3D11DeviceContext*)renderer->data.context;

	logger::info("Initializing ImGui...");

	ImGui::CreateContext();

	auto& io = ImGui::GetIO();
	io.ConfigFlags |= (ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_NoMouseCursorChange);

	io.IniFilename                       = iniFile.c_str();
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	io.MousePos                          = { io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f };

	{
		static const auto screenSize         = RE::BSGraphics::Renderer::GetSingleton()->GetScreenSize();
		io.DisplaySize.x                     = static_cast<float>(screenSize.width);
		io.DisplaySize.y                     = static_cast<float>(screenSize.height);
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		io.WantCaptureKeyboard               = true;
		io.WantCaptureMouse                  = true;
		io.MouseDrawCursor                   = false;
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	}

	if (!ImGui_ImplWin32_Init(desc.OutputWindow))
	{
		logger::error("ImGui initialization failed (Win32)");
		return;
	}
	if (!ImGui_ImplDX11_Init(device, context))
	{
		logger::error("ImGui initialization failed (DX11)");
		return;
	}

	logger::info("ImGui initialized.");
	ImGuiRenderer::installedHooks.store(true);

	WndProc::func = reinterpret_cast<WNDPROC>(
		SetWindowLongPtrA(
			desc.OutputWindow,
			GWLP_WNDPROC,
			reinterpret_cast<LONG_PTR>(WndProc::thunk)));

	if (!WndProc::func)
	{
		logger::error("SetWindowLongPtrA failed!");
	}

	logger::info("Set ImGui Style");
}

LRESULT ImGui::ImGuiRenderer::WndProc::thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto& io = ImGui::GetIO();
	if (uMsg == WM_KILLFOCUS)
	{
		io.ClearInputCharacters();
		io.ClearInputKeys();
	}
	return func(hWnd, uMsg, wParam, lParam);
}

void ImGui::ImGuiRenderer::StopTimer::thunk(std::uint32_t timer)
{
	func(timer);

	if (!installedHooks.load())
		return;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	{
		static const auto screenSize = RE::BSGraphics::Renderer::GetSingleton()->GetScreenSize();
		auto&             io         = ImGui::GetIO();
		io.DisplaySize.x             = static_cast<float>(screenSize.width);
		io.DisplaySize.y             = static_cast<float>(screenSize.height);
		io.MouseDrawCursor           = false;

		for (const auto& target : targets)
		{
			if (target->ShouldDrawCursor())
			{
				io.MouseDrawCursor = true;
				break;
			}
		}
	}

	ImGui::NewFrame();
	{
		for (const auto& target : targets)
		{
			if (target->ShouldSkip())
				continue;
			target->DoFrame();
		}
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGui::ImGuiRenderer::Init()
{
	REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(75595, 77226), OFFSET(0x9, 0x275) };  // BSGraphics::InitD3D
	stl::write_thunk_call<CreateD3DAndSwapChain>(target.address());

	REL::Relocation<std::uintptr_t> target2{ RELOCATION_ID(75461, 77246), 0x9 };  // BSGraphics::Renderer::End
	stl::write_thunk_call<StopTimer>(target2.address());
}

void ImGui::ImGuiRenderer::RegisterRenderTarget(ImGuiComponent* target)
{
	targets.insert(target);
}

void ImGui::ImGuiRenderer::UnregisterRenderTarget(ImGuiComponent* target)
{
	targets.erase(target);
}
