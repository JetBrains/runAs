#pragma once
#include <string>
#include <list>

typedef wstring ShowMode;
#define SHOW_MODE_HIDE		TEXT("hide")	// Hides the window and activates another window.
#define SHOW_MODE_NORMAL	TEXT("normal")	// Activates and displays a window. If the window is minimized or maximized, the system restores it to its original size and position. An application should specify this flag when displaying the window for the first time.
#define SHOW_MODE_SHOW		TEXT("show")	// Activates the window and displays it in its current size and position.

const list<ShowMode> ShowModes = { SHOW_MODE_HIDE, SHOW_MODE_NORMAL, SHOW_MODE_SHOW };