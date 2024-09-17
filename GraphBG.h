#pragma once
#include "Core.h"

typedef struct GraphBGObj_tag* GraphBG;
typedef struct GraphBGClassDescriptor_tag GraphBGClassDescriptor;
extern CoreClassDescriptor* graphBGClass;

void setupBG(GraphBG self, int image, int height);
