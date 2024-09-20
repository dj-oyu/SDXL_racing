#include <stdio.h>
#include <math.h>
#include "DxLib.h"
#include "framework.h"
#include "GraphCarP.h"

const double M_PI = 3.14159265358979323846264338327950288;

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
		"GraphCar",                    /* class_name */
		(CoreClassDescriptor*)&graph_base_class_descriptor,  /* super_class */
		sizeof(GraphCarObj),           /* size_of_instance */
		NULL,                         /* class_initializer */
		init,                         /* initializer */
		fin,						 /* finalizer */
	},
	/* GraphBase part */
{
	trampoline_constructor,		  /* constructor */
},
/* GraphCar part */
{
	GraphCar_new,                 /* constructor */
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

static void fin(Core p) {
	GraphCar car = (GraphCar)p;
	DeleteGraph(car->base.image);
	free(car);
	p = NULL;
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

// 画像のバウンディングボックスを計算する関数
static void calculateBoundingBox(GraphCar c, double rad, double* w, double* h) {
	int car_w = c->car.width, car_h = c->car.height;
	VECTOR topleft = VAdd(c->base.coordinates, VGet(-car_w / 2, -car_h / 2, 0)),
		  topright = VAdd(c->base.coordinates, VGet( car_w / 2, -car_h / 2, 0)),
		bottomleft = VAdd(c->base.coordinates, VGet(-car_w / 2,  car_h / 2, 0)),
	   bottomright = VAdd(c->base.coordinates, VGet( car_w / 2,  car_h / 2, 0));

	// 頂点を回転させる
	MATRIX rot = MGetRotZ(rad - M_PI/4);
	VECTOR rot_tl = VTransform(topleft,     rot),
		   rot_tr = VTransform(topright,    rot),
		   rot_bl = VTransform(bottomleft,  rot),
		   rot_br = VTransform(bottomright, rot);

	// 回転後の頂点から左上、右下の座標を計算する
	double min_x = min(min(rot_tl.x, rot_tr.x), min(rot_bl.x, rot_br.x)),
		   max_x = max(max(rot_tl.x, rot_tr.x), max(rot_bl.x, rot_br.x)),
		   min_y = min(min(rot_tl.y, rot_tr.y), min(rot_bl.y, rot_br.y)),
		   max_y = max(max(rot_tl.y, rot_tr.y), max(rot_bl.y, rot_br.y));
	*w = max_x - min_x;
	*h = max_y - min_y;
}

static int update_car(GraphBase p) {
	GraphCar car = (GraphCar)p;

	if (rand() % 20 == 0)
		rotate_car(car, rand() % 100 - 50);

	int s = car->car.speed;
	double rad = (car->car.direction - 135) * M_PI / 180;
	VECTOR m = VTransform(VGet(s, s, 0), MGetRotZ(rad));
	car->base.coordinates = VAdd(car->base.coordinates, m);

	/* calculate hit box */
	double outer_w, outer_h;
	calculateBoundingBox(car, rad, &outer_w, &outer_h);
	DrawBox(
		car->base.coordinates.x - outer_w / 2, car->base.coordinates.y - outer_h / 2,
		car->base.coordinates.x + outer_w / 2, car->base.coordinates.y + outer_h / 2,
		GetColor(255, 255, 255), FALSE);

	/* culling */
	if (car->base.coordinates.x + outer_w / 2 < 0 &&
		90 <= car->car.direction && car->car.direction <= 270) {
		return -1;
	}
	if (car->base.coordinates.x - outer_w / 2 > car->car.bg_w &&
		(car->car.direction <= 90 || 270 <= car->car.direction)) {
		return -1;
	}
	if (car->base.coordinates.y + outer_h / 2 < 0 &&
		0 <= car->car.direction && car->car.direction <= 180) {
		return -1;
	}
	if (car->base.coordinates.y - outer_h / 2 > car->car.bg_h &&
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
