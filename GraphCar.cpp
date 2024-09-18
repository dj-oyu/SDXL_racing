#include <stdio.h>
#include <math.h>
#include "DxLib.h"
#include "GraphCarP.h"

static void init(Core p);
static void fin(Core p);
static void rotate_car(GraphCar self, int theta);
static int update_car(GraphBase p);
static int draw_car(GraphBase p);

GraphCarClassDescriptor graphCar_class_descriptor = {
	/* Core part */
	{
		"GraphCar",                    /* class_name */
		(CoreClassDescriptor*)&graph_base_class_descriptor,  /* super_class */
		sizeof(GraphCarObj),           /* size_of_instance */
		NULL,                         /* class_initializer */
		init,                         /* initializer */
		fin,						 /* finalizer */
	},
		/* GraphBase part */
	{
			update_car,                    /* update */
			draw_car,                      /* draw */
	},
		/* GraphCar part */
	{
			0,							  /* dummy */
	},
};

CoreClassDescriptor* graphCarClass = (CoreClassDescriptor*)&graphCar_class_descriptor;

void setupCar(GraphCar self, int x, int y, int image, int width, int height, int speed, int direction) {
	self->base.x = x;
	self->base.y = y;
	self->base.image = image;
	self->car.bg_w = width;
	self->car.bg_h = height;
	GetGraphTextureSize(image, &self->car.width, &self->car.height);
	self->car.speed = speed;
	self->car.direction = direction;
}

static void init(Core p) {
	GraphCar car = (GraphCar)p;
	car->car.bg_w = 0;
	car->car.bg_h = 0;
	car->car.width = 0;
	car->car.height = 0;
	car->car.speed = 0;
	car->car.direction = 0;
}

static void fin(Core p) {
	GraphCar car = (GraphCar)p;
	DeleteGraph(car->base.image);
	free(car);
	p = NULL;
}

static void rotate_car(GraphCar self, int theta) {
	self->car.direction += theta;
	if (self->car.direction < 0) {
		self->car.direction += 360;
	}
	else if (self->car.direction >= 360) {
		self->car.direction -= 360;
	}
}

static int update_car(GraphBase p) {
	GraphCar car = (GraphCar)p;
	int s = car->car.speed;
	double rad = car->car.direction * 3.14159265 / 180;

	double dx = s * cos(rad); dx = dx > 0 ? ceil(dx) : floor(dx);
	double dy = s * sin(rad); dy = dy > 0 ? ceil(dy) : floor(dy);

	car->base.x += dx;
	car->base.y += dy;

	if (car->base.x + car->car.width < 0 &&
		90 <= car->car.direction && car->car.direction <= 270) {
		fin((Core)car);
		return -1;
	}
	if (car->base.x > car->car.bg_w &&
		(car->car.direction <= 90 || 270 <= car->car.direction)) {
		fin((Core)car);
		return -1;
	}
	if (car->base.y + car->car.height < 0 &&
		0 <= car->car.direction && car->car.direction <= 180) {
		fin((Core)car);
		return -1;
	}
	if (car->base.y > car->car.bg_h &&
		180 <= car->car.direction && car->car.direction <= 360) {
		fin((Core)car);
		return -1;
	}
	return 0;
}

static int draw_car(GraphBase p) {
	GraphCar car = (GraphCar)p;
	double rad = car->car.direction * 3.14159265 / 180;
	return DrawRotaGraph(
		car->base.x,
		car->base.y,
		1.0, rad,
		car->base.image, TRUE);
}
