#pragma once
#include "CoreP.h"
#include "GraphBaseP.h"
#include "GraphBG.h"

typedef struct {
	char dummy;
} GraphBGClassDescriptorPart;

struct  GraphBGClassDescriptor_tag {
	CoreClassDescriptorPart core;
	GraphBaseClassDescriptorPart base;
	GraphBGClassDescriptorPart bg;
};

typedef struct {
	int height;
} GraphBGPart;

typedef struct GraphBGObj_tag {
	CorePart core;
	GraphBasePart base;
	GraphBGPart bg;
} GraphBGObj;
