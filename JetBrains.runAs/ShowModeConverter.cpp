#include "stdafx.h"
#include "ShowModeConverter.h"

WORD ShowModeConverter::ToShowWindowFlag(const ShowMode& showMode)
{
	if (showMode == SHOW_MODE_HIDE)
	{
		return SW_HIDE;
	}

	if (showMode == SHOW_MODE_NORMAL)
	{
		return SW_NORMAL;
	}

	if (showMode == SHOW_MODE_SHOW)
	{
		return SW_SHOW;
	}

	return 0;
}
