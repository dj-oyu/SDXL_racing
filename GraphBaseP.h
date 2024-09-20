#pragma once
#include "CoreP.h"
#include "GraphBase.h"

typedef struct {
	/* Todo: ������va_list�^�� */
	GraphBase(*constructor)(int handle, va_list* ap);
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