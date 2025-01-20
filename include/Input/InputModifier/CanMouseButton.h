#pragma once
#include "InputModifier.h"

namespace Input
{
	class CanMouseButton :
		public InputModifier
	{
	public:
		CanMouseButton(bool blockMouseButtons, int slot);

	public:
		MouseSupressionMask ApplyMouseKeyModifier(MouseSupressionMask mouse) const override;

	private:
		bool blockMouseButtons;
	};
}
