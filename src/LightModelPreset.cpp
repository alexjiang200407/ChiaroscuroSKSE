#include "LightModelPreset.h"

using namespace preset;

LightModelPreset::LightModelPreset(PresetID id, std::string name, RE::FormID modelSubID, std::string file, RE::NiPoint3 offset) :
	ModelPreset(id, name, modelSubID, file), offset(offset)
{
}

RE::NiPoint3 preset::LightModelPreset::GetOffset() const
{
	return offset;
}

PresetPtr LightModelPreset::Deserializer::operator()(PresetID id, std::string name, json json) const
{
	if (!json.contains("formID") || !json.contains("file") || !json.contains("offset"))
		throw std::runtime_error("LightModelPreset must include formID, file and offset field");

	if (!json["offset"].is_array())
		throw std::runtime_error("ModelPreset must include formID, file and offset field");

	RE::FormID  formID = json["formID"];
	std::string file   = json["file"];
	RE::NiPoint3 offset;
	
	offset.x = json["offset"][0];
	offset.y = json["offset"][1];
	offset.z = json["offset"][2];
	
	return std::make_unique<LightModelPreset>(LightModelPreset(id, name, formID, file, offset));
}
