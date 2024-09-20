#pragma once
#include "CoreP.h"
#include "GraphBaseP.h"
#include "GraphCarP.h"
#include "GraphBBCar.h"

typedef struct {
	GraphBBCar(*constructor)(int x, int y, int image, int width, int height, int speed, int direction);
} GraphBBCarClassDescriptorPart;

struct GraphBBCarClassDescriptor_tag {
	CoreClassDescriptorPart core;
	GraphBaseClassDescriptorPart base;
	GraphCarClassDescriptorPart car;
	GraphBBCarClassDescriptorPart bbc;
};

typedef struct {
	VECTOR outer_box[2];
	int (*intersect)(GraphBBCar self, GraphBBCar other);
	void(*calc_outer_box)(GraphBBCar self, double rad, VECTOR* tl, VECTOR* br);
} GraphBBCarPart;

typedef struct GraphBBCarObj_tag {
	CorePart core;
	GraphBasePart base;
	GraphCarPart car;
	GraphBBCarPart bbc;
} GraphBBCarObj;

extern GraphBBCarClassDescriptor graphBBCar_class_descriptor;
