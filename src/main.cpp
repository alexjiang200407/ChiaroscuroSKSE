

void InitializeLog()
{
	auto path = logger::log_directory();
	if (!path)
	{
		SKSE::stl::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= Version::PROJECT;
	*path += ".log"sv;
	auto sink =
		std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	auto logLevel = spdlog::level::trace;

	log->set_level(logLevel);
	log->flush_on(logLevel);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S:%e] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);
}

#ifdef SKYRIM_AE
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;
	v.PluginVersion(Version::MAJOR);
	v.PluginName("CineStudio");
	v.AuthorName("shdowraithe101");
	v.UsesAddressLibrary();
	v.UsesUpdatedStructs();
	v.CompatibleVersions({ SKSE::RUNTIME_LATEST });

	return v;
}();
#else
extern "C" DLLEXPORT bool SKSEAPI
	SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name        = "CineStudio";
	a_info->version     = Version::MAJOR;

	if (a_skse->IsEditor())
	{
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver
#	ifndef SKYRIMVR
	    < SKSE::RUNTIME_1_5_39
#	else
	    > SKSE::RUNTIME_VR_1_4_15_1
#	endif
	)
	{
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}
#endif

namespace RE
{
	template <class T>
	void UpdateNode(T* a_obj)
	{
		if (TaskQueueInterface::ShouldUseTaskQueue())
		{
			QueueUpdateNiObject(TaskQueueInterface::GetSingleton(), a_obj);
		}
		else
		{
			NiUpdateData updateData;
			a_obj->Update(updateData);
		}
	}

	void QueueUpdateNiObject(TaskQueueInterface* self, NiAVObject* a_obj)
	{
		using func_t = decltype(QueueUpdateNiObject);
		static REL::Relocation<func_t> func{ RELOCATION_ID(35929, 36904) };
		return func(self, a_obj);
	}
}  // namespace RE

class PoC : public RE::BSTEventSink<RE::InputEvent*>
{
public:
	virtual RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>*) override
	{
		if (!a_event || !*a_event)
			return RE::BSEventNotifyControl::kContinue;

		auto* buttonEvt = (*a_event)->AsButtonEvent();

		if (!buttonEvt)
			return RE::BSEventNotifyControl::kContinue;
		
		if (buttonEvt->GetIDCode() != 207 || !buttonEvt->IsUp())
			return RE::BSEventNotifyControl::kContinue;

		// END Pressed create a light at player location

		const auto       dataHandler = RE::TESDataHandler::GetSingleton();
		const RE::FormID id          = dataHandler->LookupFormID(0x800, "CinematographyStudio.esp");

		auto* lightObjRef = RE::PlayerCharacter::GetSingleton()->PlaceObjectAtMe(RE::TESForm::LookupByID(id)->As<RE::TESBoundObject>(), true).get();
		lightObjRef->Load3D(false);

		auto niRoot = lightObjRef->Get3D()->AsFadeNode();
		for (const auto& a : niRoot->children)
		{
			logger::info("{}", a->name.c_str());
		}

		auto niAvNode = niRoot->children[0].get();

		auto* light = skyrim_cast<RE::NiLight*>(niAvNode);

		light->diffuse.red   = 0;
		light->diffuse.blue  = 0.5f;
		light->diffuse.green = 0;
		light->radius        = RE::NiPoint3(500, 500, 500);
		light->fade          = 2;
	}
};

static PoC singleton;

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	InitializeLog();

	logger::info("Game version : {}", a_skse->RuntimeVersion().string());

	SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
			if (message->type == SKSE::MessagingInterface::kDataLoaded)
			{
				RE::ConsoleLog::GetSingleton()->Print("CineStudio has been loaded");
				RE::BSInputDeviceManager::GetSingleton()->AddEventSink(&singleton);
			}
		});

	return true;
}
