#pragma once
#include "CoreP.h"
#include "GraphBase.h"

typedef struct {
	GraphBase(*constructor)(int n_args, ...);
} GraphBaseClassDescriptorPart;

struct GraphBaseClassDescriptor_tag {
	CoreClassDescriptorPart core;
	GraphBaseClassDescriptorPart base;
};

typedef struct {
	int x, y;
	int image;
	int (*update)(GraphBase);
	int (*draw)(GraphBase);
} GraphBasePart;

typedef struct GraphBaseObj_tag {
	CorePart core;
	GraphBasePart base;
} GraphBaseObj;

extern GraphBaseClassDescriptor graph_base_class_descriptor;