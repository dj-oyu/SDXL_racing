#include <stdio.h>
#include <math.h>
#include "DxLib.h"
#include "framework.h"
#include "GraphCarP.h"

static void init(Core p);
static void fin(Core p);
static void rotate_car(GraphCar self, int theta);
static int update_car(GraphBase p);
static int draw_car(GraphBase p);

static GraphCar GraphCar_new(int x, int y, int image, int width, int height, int speed, int direction) {
	GraphCar car = (GraphCar)new_instance(graphCarClass);
	car->base.coordinates.x = x;
	car->base.coordinates.y = y;
	car->base.image = image;
	car->car.bg_w = width;
	car->car.bg_h = height;
	GetGraphSize(image, &car->car.width, &car->car.height);
	car->car.speed = speed;
	car->car.direction = direction;
	return car;
}

static GraphBase trampoline_constructor(int handle, va_list* ap) {
	va_list cpy;
	va_copy(cpy, *ap);

	int x = va_arg(cpy, int);
	int y = va_arg(cpy, int);
	int width = va_arg(cpy, int);
	int height = va_arg(cpy, int);
	int speed = va_arg(cpy, int);
	int direction = va_arg(cpy, int);

	va_end(cpy);

	return (GraphBase)GraphCar_new(x, y, handle, width, height, speed, direction);
}

GraphCarClassDescriptor graphCar_class_descriptor = {
	/* Core part */
	{
		"GraphCar",                      /* class_name */
		(CoreClassDescriptor*)&graph_base_class_descriptor,  /* super_class */
		sizeof(GraphCarObj),             /* size_of_instance */
		NULL,                            /* class_initializer */
		init,                            /* initializer */
		graphBaseClass->core.finalizer,	 /* finalizer */
	},
	/* GraphBase part */
	{
		trampoline_constructor,		     /* constructor */
	},
	/* GraphCar part */
	{
		GraphCar_new,                    /* constructor */
	},
};

CoreClassDescriptor* graphCarClass = (CoreClassDescriptor*)&graphCar_class_descriptor;

static void init(Core p) {
	GraphCar car = (GraphCar)p;
	car->base.draw = draw_car;
	car->base.update = update_car;

	car->car.bg_w = 0;
	car->car.bg_h = 0;
	car->car.width = 0;
	car->car.height = 0;
	car->car.speed = 0;
	car->car.direction = 0;

	car->car.rotate = rotate_car;
}

static void rotate_car(GraphCar self, int theta) {
	self->car.direction += theta;
	while (self->car.direction < 0) {
		self->car.direction += 360;
	}
	while (self->car.direction >= 360) {
		self->car.direction -= 360;
	}
}

static int update_car(GraphBase p) {
	GraphCar car = (GraphCar)p;

	if (rand() % 20 == 0)
		rotate_car(car, rand() % 100 - 50);

	int s = car->car.speed;
	double rad = (car->car.direction - 135) * M_PI / 180;
	VECTOR m = VTransform(VGet(s, s, 0), MGetRotZ(rad));
	car->base.coordinates = VAdd(car->base.coordinates, m);

	int outer_circle = max(car->car.width, car->car.height);

	/* culling */
	if (car->base.coordinates.x + outer_circle < 0 &&
		90 <= car->car.direction && car->car.direction <= 270) {
		return -1;
	}
	if (car->base.coordinates.x - outer_circle > car->car.bg_w &&
		(car->car.direction <= 90 || 270 <= car->car.direction)) {
		return -1;
	}
	if (car->base.coordinates.y + outer_circle < 0 &&
		0 <= car->car.direction && car->car.direction <= 180) {
		return -1;
	}
	if (car->base.coordinates.y - outer_circle > car->car.bg_h &&
		180 <= car->car.direction && car->car.direction <= 360) {
		return -1;
	}
	return 0;
}

static int draw_car(GraphBase p) {
	GraphCar car = (GraphCar)p;
	double rad = car->car.direction * M_PI / 180;
	return DrawRotaGraph(
		car->base.coordinates.x,
		car->base.coordinates.y,
		1.0, rad,
		car->base.image, TRUE);
}
