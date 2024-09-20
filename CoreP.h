#pragma once
#include "Core.h"

typedef struct {
	CoreClassDescriptor* class_descriptor;
} CorePart;

typedef struct CoreObj_tag{
	CorePart core;
} CoreObj;

typedef struct {
	const char*          class_name;
	CoreClassDescriptor* super;
	int                  size;
	void                 (*class_initializer)(void);
	void                 (*initializer)(Core);
	void                 (*finalizer)(Core);
} CoreClassDescriptorPart;

struct CoreClassDescriptor_tag {
	CoreClassDescriptorPart core;
};

extern CoreClassDescriptor core_class_descriptor;
