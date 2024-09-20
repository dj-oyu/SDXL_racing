#include <math.h>
#include "DxLib.h"
#include "framework.h"
#include "GraphBBCarP.h"

static void init(Core p);
static void fin(Core p);
static int update_car(GraphBase p);
static int intersect(GraphBBCar self, GraphBBCar other);
static void calc_outer_box(GraphBBCar self, double rad, VECTOR* tl, VECTOR* br);

static GraphBBCar GraphBBCar_new(int x, int y, int image, int width, int height, int speed, int direction) {
	GraphBBCar car = (GraphBBCar)new_instance(graphBBCarClass);
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

	return (GraphBase)GraphBBCar_new(x, y, handle, width, height, speed, direction);
}

GraphBBCarClassDescriptor graphBBCar_class_descriptor = {
	/* Core part */
	{
		"GraphBBCar",                     /* class_name */
		(CoreClassDescriptor*)&graphCar_class_descriptor,  /* super_class */
		sizeof(GraphBBCarObj),            /* size_of_instance */
		NULL,                             /* class_initializer */
		init,                             /* initializer */
		graphBaseClass->core.finalizer,   /* finalizer */
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

// 画像のバウンディングボックスを計算する関数
static void calc_outer_box(GraphBBCar self, double rad, VECTOR* tl, VECTOR* br) {
	int car_w = self->car.width / 2, car_h = self->car.height / 2;
	VECTOR topleft = VGet(-car_w, -car_h, 0),
		   topright = VGet(car_w, -car_h, 0),
		   bottomleft = VGet(-car_w, car_h, 0),
		   bottomright = VGet(car_w, car_h, 0);

	// 頂点を回転させる
	MATRIX rot = MGetRotZ(rad);
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

	// 回転後の頂点から内接矩形の座標を計算する
	VECTOR* inner_box = self->bbc.inner_box;
	inner_box[0] = VAdd(rot_tl, self->base.coordinates);
	inner_box[1] = VAdd(rot_tr, self->base.coordinates);
	inner_box[2] = VAdd(rot_br, self->base.coordinates);
	inner_box[3] = VAdd(rot_bl, self->base.coordinates);

	/* test */
	DrawLine(inner_box[0].x, inner_box[0].y, inner_box[3].x, inner_box[3].y, GetColor(0, 255, 0), 3);
	DrawLine(inner_box[0].x, inner_box[0].y, inner_box[1].x, inner_box[1].y, GetColor(0, 255, 0), 3);

	DrawBox(tl->x, tl->y, br->x, br->y,GetColor(255, 255, 255), FALSE);
}

static int update_car(GraphBase p) {
	GraphCar car = (GraphCar)p;
	GraphBBCar bbc = (GraphBBCar)p;

	if (rand() % 20 == 0)
		car->car.rotate(car, rand() % 100 - 50);

	int s = car->car.speed;
	double rad = (car->car.direction) * M_PI / 180;
	VECTOR move = VGet(sin(rad) * s, -cos(rad) * s, 0);

	car->base.coordinates = VAdd(car->base.coordinates, move);

	/* calculate hit box */
	VECTOR* outer_box = bbc->bbc.outer_box;
	calc_outer_box(bbc, rad, &outer_box[0], &outer_box[1]);

	/* culling */
	if (outer_box[1].x < 0 &&
		90 <= car->car.direction && car->car.direction <= 270) {
		return -1;
	}
	if (outer_box[0].x > car->car.bg_w &&
		(car->car.direction <= 90 || 270 <= car->car.direction)) {
		return -1;
	}
	if (outer_box[1].y < 0 &&
		0 <= car->car.direction && car->car.direction <= 180) {
		return -1;
	}
	if (outer_box[0].y > car->car.bg_h &&
		180 <= car->car.direction && car->car.direction <= 360) {
		return -1;
	}
	return 0;
}

static int intersect(GraphBBCar self, GraphBBCar other) {
	VECTOR* me_outer = self->bbc.outer_box;
	VECTOR* you_outer = other->bbc.outer_box;

	// 外接矩形同士の当たり判定
	if (you_outer[1].x < me_outer[0].x || me_outer[1].x < you_outer[0].x ||
		you_outer[1].y < me_outer[0].y || me_outer[1].y < you_outer[0].y) {
		return 0;
	}

	// 内接矩形同士の当たり判定 SATで判定
	VECTOR* me_inner = self->bbc.inner_box;
	VECTOR* you_inner = other->bbc.inner_box;

	VECTOR norm[2][4];
	for (int i = 0; i < 4; i++) {
		norm[0][i] = VNorm(VSub(me_inner[(i + 1) % 4], me_inner[i]));
		norm[1][i] = VNorm(VSub(you_inner[(i + 1) % 4], you_inner[i]));
	}

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4; j++) {
			double min_me = VDot(norm[i][j], me_inner[0]),
				max_me = min_me;
			double min_you = VDot(norm[i][j], you_inner[0]),
				max_you = min_you;

			for (int k = 1; k < 4; k++) {
				double d = VDot(norm[i][j], me_inner[k]);
				if (d < min_me) min_me = d;
				if (d > max_me) max_me = d;

				d = VDot(norm[i][j], you_inner[k]);
				if (d < min_you) min_you = d;
				if (d > max_you) max_you = d;
			}

			if (max_me < min_you || max_you < min_me) {
				return 0;
			}
		}
	}
	return 1;
}