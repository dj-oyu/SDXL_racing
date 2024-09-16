#pragma once
#include "Core.h"

typedef struct GraphBGObj_tag* GraphBG;
typedef struct GraphBGClassDescriptor_tag GraphBGClassDescriptor;
CoreClassDescriptor* graphBGClass;

CoreClassDescriptor* setupBG(int height, const char* imagePath);
