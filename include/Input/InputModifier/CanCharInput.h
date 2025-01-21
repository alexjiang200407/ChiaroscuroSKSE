#pragma once
#include "InputModifier.h"

namespace Input
{
	class CanCharInput :
		public InputModifier
	{
	public:
		CanCharInput(bool blockCharModifer);

	public:
		bool ApplyBlockCharModifier(bool prevBockCharModifier) const override;

	private:
		bool blockCharModifer;
	};
}
