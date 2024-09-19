#pragma once
#include "CoreP.h"
#include "GraphBaseP.h"
#include "GraphCar.h"

typedef struct {
	char dummy;
} GraphCarClassDescriptorPart;

struct  GraphCarClassDescriptor_tag {
	CoreClassDescriptorPart core;
	GraphBaseClassDescriptorPart base;
	GraphCarClassDescriptorPart bg;
};

typedef struct {
	int bg_w, // background width
		bg_h, // background height
		width, // car width
		height, // car height
		speed, // car speed
		direction; // degree
	void (*spawn_car)(GraphCar self, int x, int y, int image, int width, int height, int speed, int direction);
	void (*rotate)(GraphCar self, int theta);
} GraphCarPart;

typedef struct GraphCarObj_tag {
	CorePart core;
	GraphBasePart base;
	GraphCarPart car;
} GraphCarObj;
