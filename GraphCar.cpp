#include <stdio.h>
#include <math.h>
#include "DxLib.h"
#include "GraphCarP.h"

const double M_PI = 3.14159265358979323846;

static void init(Core p);
static void fin(Core p);
static void rotate_car(GraphCar self, int theta);
static int update_car(GraphBase p);
static int draw_car(GraphBase p);
static void spawn_car(GraphCar self, int x, int y, int image, int width, int height, int speed, int direction);
static void rotate_car(GraphCar self, int theta);

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
		0,							  /* dummy */
	},
		/* GraphCar part */
	{
		0,							  /* dummy */
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

	car->car.spawn_car = spawn_car;
	car->car.rotate = rotate_car;
}

static void fin(Core p) {
	GraphCar car = (GraphCar)p;
	DeleteGraph(car->base.image);
	free(car);
	p = NULL;
}

static void spawn_car(GraphCar self, int x, int y, int image, int width, int height, int speed, int direction) {
	self->base.x = x;
	self->base.y = y;
	self->base.image = image;
	self->car.bg_w = width;
	self->car.bg_h = height;
	GetGraphSize(image, &self->car.width, &self->car.height);
	self->car.speed = speed;
	self->car.direction = direction;
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
static void calculateBoundingBox(double width, double height, double angle, double& outWidth, double& outHeight) {
	double rad = angle * M_PI / 180.0;

	// 各頂点の座標を計算
	double x1 = 0;      double x2 = width;
	double y1 = 0;      double y2 = 0;
	
	double x3 = 0;      double x4 = width;
	double y3 = height; double y4 = height;

	// 回転後の各頂点の座標を計算
	double newX1 = x1 * cos(rad) - y1 * sin(rad);
	double newY1 = x1 * sin(rad) + y1 * cos(rad);
	double newX2 = x2 * cos(rad) - y2 * sin(rad);
	double newY2 = x2 * sin(rad) + y2 * cos(rad);
	double newX3 = x3 * cos(rad) - y3 * sin(rad);
	double newY3 = x3 * sin(rad) + y3 * cos(rad);
	double newX4 = x4 * cos(rad) - y4 * sin(rad);
	double newY4 = x4 * sin(rad) + y4 * cos(rad);

	// バウンディングボックスの幅と高さを計算
	double minX = min(min(newX1, newX2), min(newX3, newX4));
	double maxX = max(max(newX1, newX2), max(newX3, newX4));
	double minY = min(min(newY1, newY2), min(newY3, newY4));
	double maxY = max(max(newY1, newY2), max(newY3, newY4));

	outWidth = maxX - minX;
	outHeight = maxY - minY;
}

static int update_car(GraphBase p) {
	GraphCar car = (GraphCar)p;

	if(rand()%20 == 0)
		rotate_car(car, rand()%100-50);

	int s = car->car.speed;
	double rad = (car->car.direction + 225) * M_PI / 180;

	double dx = s * (cos(rad) - sin(rad));
	double dy = s * (sin(rad) + cos(rad));

	car->base.x += dx;
	car->base.y += dy;

	double outer_w, outer_h;
	calculateBoundingBox(car->car.width, car->car.height, car->car.direction, outer_w, outer_h);
	DrawBox(
		car->base.x - outer_w / 2, car->base.y - outer_h / 2,
		car->base.x + outer_w / 2, car->base.y + outer_h / 2,
		GetColor(255, 255, 255), FALSE);

	if (car->base.x + outer_w / 2 < 0 &&
		90 <= car->car.direction && car->car.direction <= 270) {
		fin((Core)car);
		return -1;
	}
	if (car->base.x - outer_w / 2 > car->car.bg_w &&
		(car->car.direction <= 90 || 270 <= car->car.direction)) {
		fin((Core)car);
		return -1;
	}
	if (car->base.y + outer_h / 2 < 0 &&
		0 <= car->car.direction && car->car.direction <= 180) {
		fin((Core)car);
		return -1;
	}
	if (car->base.y - outer_h / 2 > car->car.bg_h &&
		180 <= car->car.direction && car->car.direction <= 360) {
		fin((Core)car);
		return -1;
	}
	return 0;
}

static int draw_car(GraphBase p) {
	GraphCar car = (GraphCar)p;
	double rad = car->car.direction * M_PI / 180;
	return DrawRotaGraph(
		car->base.x,
		car->base.y,
		1.0, rad,
		car->base.image, TRUE);
}
