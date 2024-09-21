#pragma once
#include "DxLib.h"
#include "CoreP.h"
#include "GraphBase.h"
#include "GraphManager.h"

typedef struct {
	GraphBase(*constructor)(int handle, va_list* ap);
} GraphBaseClassDescriptorPart;

struct GraphBaseClassDescriptor_tag {
	CoreClassDescriptorPart core;
	GraphBaseClassDescriptorPart base;
};

typedef struct {
	VECTOR coordinates;
	int image;
	int (*update)(GraphBase);
	int (*draw)(GraphBase);
	void (*finish_draw)(GraphBase, GraphManager);
} GraphBasePart;

typedef struct GraphBaseObj_tag {
	CorePart core;
	GraphBasePart base;
} GraphBaseObj;

extern GraphBaseClassDescriptor graph_base_class_descriptor;