#pragma once
#include "Core.h"

typedef struct GraphBaseObj_tag* GraphBase;
typedef struct GraphBaseClassDescriptor_tag GraphBaseClassDescriptor;
extern CoreClassDescriptor* graphBaseClass;

int update(GraphBase base);
int draw(GraphBase base);
