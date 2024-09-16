#pragma once
#include "CoreP.h"
#include "GraphBaseP.h"
#include "GraphBG.h"

typedef struct {
	int height;
	const char* imagePath;
} GraphBGClassDescriptorPart;

struct  GraphBGClassDescriptor_tag {
	CoreClassDescriptorPart core;
	GraphBaseClassDescriptorPart base;
	GraphBGClassDescriptorPart bg;
};

typedef struct {
	int width, height;
	const char* imagePath;
} GraphBGPart;

typedef struct GraphBGObj_tag {
	CorePart core;
	GraphBasePart base;
	GraphBGPart bg;
} GraphBGObj;
