#pragma once
#include <string>

typedef wstring InheritanceMode;
#define INHERITANCE_MODE_OFF		TEXT("off")
#define INHERITANCE_MODE_ON			TEXT("on")
#define INHERITANCE_MODE_AUTO		TEXT("auto")

const list<InheritanceMode> InheritanceModes = { INHERITANCE_MODE_OFF, INHERITANCE_MODE_ON, INHERITANCE_MODE_AUTO };