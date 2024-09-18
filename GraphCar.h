#pragma once
#include "Core.h"

typedef struct GraphCarObj_tag* GraphCar;
typedef struct GraphCarClassDescriptor_tag GraphCarClassDescriptor;
extern CoreClassDescriptor* graphCarClass;

void setupCar(GraphCar self, int x, int y, int image, int width, int height, int speed, int direction);
