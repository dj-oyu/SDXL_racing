#pragma once
#include "Core.h"

typedef struct GraphBaseObj_tag* GraphBase;
typedef struct GraphBaseClassDescriptor_tag GraphBaseClassDescriptor;
extern CoreClassDescriptor* graphBaseClass;

void update(GraphBase base);
void draw(GraphBase base);
