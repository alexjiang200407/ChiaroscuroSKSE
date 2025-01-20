#pragma once

#define WIN32_LEAN_AND_MEAN

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#include "imgui_internal.h"
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include "ImGui/ImGuiWidgets.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <xbyak/xbyak.h>

#define DLLEXPORT __declspec(dllexport)

namespace logger = SKSE::log;

using namespace std::literals;

#ifdef SKYRIM_AE
#	define OFFSET(se, ae)       ae
#	define OFFSET_3(se, ae, vr) ae
#elif SKYRIMVR
#	define OFFSET(se, ae)       se
#	define OFFSET_3(se, ae, vr) vr
#else
#	define OFFSET(se, ae)       se
#	define OFFSET_3(se, ae, vr) se
#endif

namespace stl
{
	using namespace SKSE::stl;

	template <class T>
	void write_thunk_call(std::uintptr_t a_src)
	{
		SKSE::AllocTrampoline(14);

		auto& trampoline = SKSE::GetTrampoline();
		T::func          = trampoline.write_call<5>(a_src, T::thunk);
	}
}

#include "Version.h"
#include "RE.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;


#define UUID_SYSTEM_GENERATOR
#include "common/uuid.h"
#include "common/util.h"

