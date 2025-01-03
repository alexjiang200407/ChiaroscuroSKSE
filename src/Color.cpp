#include "Color.h"

Color::Color(PresetID id, std::string name, RE::NiColor color) :
	Preset(id, name), color(color)
{
}

Color::Color(std::string name, RE::NiColor color) :
	Preset(PresetID::GenID<Color>(), name), color(color)
{}


RE::NiColor Color::GetColor() const
{
	return color;
}

PresetPtr Color::Deserializer::operator()(PresetID id, std::string name, nlohmann::json json) const
{
	uint32_t color = json["colorcode"];
	return std::make_unique<Color>(Color(id, name, RE::NiColor(color)));
}
