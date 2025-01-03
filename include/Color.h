#pragma once
#include "Preset/Preset.h"
#include "Preset/SerializationStrategy.h"

namespace preset
{
	class Color :
		public Preset
	{
	public:
		class Deserializer :
			public DeserializationStrategy
		{
		public:
			PresetPtr operator()(PresetID a_id, std::string a_name, json json) const override;
		};

	public:
		Color(PresetID a_id, std::string a_name, RE::NiColor color);
		Color(std::string name, RE::NiColor color);

		operator RE::NiColor() const;

	public:
		RE::NiColor GetColor() const;


	public:
		static constexpr PresetTID TID = PresetTID::kColor;

	private:
		RE::NiColor color;
	};
}
