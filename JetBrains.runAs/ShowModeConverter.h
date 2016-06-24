#pragma once
#include "ShowMode.h"

class ShowModeConverter
{
public:
	static WORD ToShowWindowFlag(const ShowMode& showMode);
};

