#pragma once
#include "CoreP.h"
#include "GraphBaseP.h"
#include "GraphCar.h"

const double M_PI = 3.14159265358979323846264338327950288;

typedef struct {
	GraphCar(*constructor)(int x, int y, int image, int width, int height, int speed, int direction);
} GraphCarClassDescriptorPart;

struct  GraphCarClassDescriptor_tag {
	CoreClassDescriptorPart core;
	GraphBaseClassDescriptorPart base;
	GraphCarClassDescriptorPart car;
};

typedef struct {
	int bg_w, // background width
		bg_h, // background height
		width, // car width
		height, // car height
		speed, // car speed
		direction; // degree
	void (*rotate)(GraphCar self, int theta);
} GraphCarPart;

typedef struct GraphCarObj_tag {
	CorePart core;
	GraphBasePart base;
	GraphCarPart car;
} GraphCarObj;

extern GraphCarClassDescriptor graphCar_class_descriptor;
