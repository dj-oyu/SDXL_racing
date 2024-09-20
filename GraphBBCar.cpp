#include "DxLib.h"
#include "framework.h"
#include "GraphBBCarP.h"

static void init(Core p);
static void fin(Core p);
static int update_car(GraphBase p);
static int intersect(GraphBBCar self, GraphBBCar other);
static void calc_outer_box(GraphBBCar self, double rad, VECTOR* tl, VECTOR* br);

static GraphBBCar GraphBBCar_new(int x, int y, int image, int width, int height, int speed, int direction) {
	return (GraphBBCar)((GraphCarClassDescriptor*)graphCarClass)->
		car.constructor(x, y, image, width, height, speed, direction);
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

	return (GraphBase)GraphBBCar_new(x, y, handle, width, height, speed, direction);
}

GraphBBCarClassDescriptor graphBBCar_class_descriptor = {
	/* Core part */
	{
		"GraphBBCar",                     /* class_name */
		(CoreClassDescriptor*)&graphCarClass,  /* super_class */
		sizeof(GraphBBCarObj),            /* size_of_instance */
		NULL,                             /* class_initializer */
		init,                             /* initializer */
		fin,						      /* finalizer */
	},
		/* GraphBase part */
	{
			trampoline_constructor,		  /* constructor */
	},
		/* GraphCar part */
	{
		NULL,                             /* constructor */
	},
	/* GraphBBCar part */
	{
		GraphBBCar_new, 			      /* constructor */
	},
};

CoreClassDescriptor* graphBBCarClass = (CoreClassDescriptor*)&graphBBCar_class_descriptor;

static void init(Core p) {
	GraphBBCar bbc = (GraphBBCar)p;
	bbc->base.update = update_car;

	bbc->bbc.calc_outer_box = calc_outer_box;
	bbc->bbc.intersect = intersect;
}

static void fin(Core p) {
}

// 画像のバウンディングボックスを計算する関数
static void calc_outer_box(GraphBBCar self, double rad, VECTOR* tl, VECTOR* br) {
	int car_w = self->car.width, car_h = self->car.height;
	VECTOR topleft = VGet(0, 0, 0),
		topright = VGet(car_w, 0, 0),
		bottomleft = VGet(0, car_h, 0),
		bottomright = VGet(car_w, car_h, 0);

	// 頂点を回転させる
	MATRIX rot = MGetRotZ(rad - M_PI / 4);
	VECTOR rot_tl = VTransform(topleft, rot),
		rot_tr = VTransform(topright, rot),
		rot_bl = VTransform(bottomleft, rot),
		rot_br = VTransform(bottomright, rot);

	// 回転後の頂点から左上、右下の座標を計算する
	double min_x = min(min(rot_tl.x, rot_tr.x), min(rot_bl.x, rot_br.x)),
		max_x = max(max(rot_tl.x, rot_tr.x), max(rot_bl.x, rot_br.x)),
		min_y = min(min(rot_tl.y, rot_tr.y), min(rot_bl.y, rot_br.y)),
		max_y = max(max(rot_tl.y, rot_tr.y), max(rot_bl.y, rot_br.y));

	int w = (max_x - min_x) / 2, h = (max_y - min_y) / 2;
	*tl = VGet(self->base.coordinates.x - w, self->base.coordinates.y - h, 0);
	*br = VGet(self->base.coordinates.x + w, self->base.coordinates.y + h, 0);
}

static int update_car(GraphBase p) {
	GraphCar car = (GraphCar)p;
	GraphBBCar bbc = (GraphBBCar)p;

	if (rand() % 20 == 0)
		car->car.rotate(car, rand() % 100 - 50);

	int s = car->car.speed;
	double rad = (car->car.direction - 135) * M_PI / 180;
	VECTOR m = VTransform(VGet(s, s, 0), MGetRotZ(rad));
	car->base.coordinates = VAdd(car->base.coordinates, m);

	/* calculate hit box */
	VECTOR tl, br;
	calc_outer_box(bbc, rad, &tl, &br);
	DrawBox(
		tl.x, tl.y, br.x, br.y,
		GetColor(255, 255, 255), FALSE);

	/* culling */
	if (br.x < 0 &&
		90 <= car->car.direction && car->car.direction <= 270) {
		return -1;
	}
	if (tl.x > car->car.bg_w &&
		(car->car.direction <= 90 || 270 <= car->car.direction)) {
		return -1;
	}
	if (tl.y < 0 &&
		0 <= car->car.direction && car->car.direction <= 180) {
		return -1;
	}
	if (br.y > car->car.bg_h &&
		180 <= car->car.direction && car->car.direction <= 360) {
		return -1;
	}
	return 0;
}

static int intersect(GraphBBCar self, GraphBBCar other) {
	return 0;
}