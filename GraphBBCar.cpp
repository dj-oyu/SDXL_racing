#include <math.h>
#include "DxLib.h"
#include "framework.h"
#include "GraphNodeP.h"
#include "GraphBBCarP.h"

#define DEFAULT_LIFETIME 90
const double LOWEST_SPEED = 1e-3;

static void init(Core p);
static void fin(Core p);
static int update_car(GraphBase p);
static int intersect(GraphBBCar self, GraphBBCar other);
static void calc_outer_box(GraphBBCar self, double rad, VECTOR* tl, VECTOR* br);
static void collide(GraphBase self, GraphManager manager);

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

	car->bbc.weight = car->car.width * car->car.height / 200;
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
	bbc->base.finish_draw = collide;

	bbc->bbc.lifetime = DEFAULT_LIFETIME;
	bbc->bbc.weight = 1;
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
	/*DrawLine(inner_box[0].x, inner_box[0].y, inner_box[3].x, inner_box[3].y, GetColor(0, 255, 0), 3);
	DrawLine(inner_box[0].x, inner_box[0].y, inner_box[1].x, inner_box[1].y, GetColor(0, 255, 0), 3);

	DrawBox(tl->x, tl->y, br->x, br->y,GetColor(255, 255, 255), FALSE);*/
}

static int update_car(GraphBase p) {
	GraphCar car = (GraphCar)p;
	GraphBBCar bbc = (GraphBBCar)p;

	if (car->car.speed <= LOWEST_SPEED) {
		bbc->bbc.lifetime--;
	}
	else {
		bbc->bbc.lifetime = DEFAULT_LIFETIME;
	}
	if (bbc->bbc.lifetime <= 0) {
		return -1;
	}

	if (rand() % 20 == 0)
		car->car.rotate(car, rand() % 100 - 50);

	int s = car->car.speed;
	double rad = (car->car.direction) * M_PI / 180;
	VECTOR move = VGet(sin(rad) * s, -cos(rad) * s, 0);

	car->base.coordinates = VAdd(car->base.coordinates, move);

	/* calculate hit box */
	VECTOR* outer_box = bbc->bbc.outer_box;
	calc_outer_box(bbc, rad, &outer_box[0], &outer_box[1]);

	/*
	*  degree
	*     0
	* 270 + 90
	*    180
	*/
	/* culling */
	if (outer_box[1].x < 0 &&
		180 <= car->car.direction && car->car.direction <= 360) {
		return -1;
	}
	if (outer_box[0].x > car->car.bg_w &&
		0 <= car->car.direction && car->car.direction <= 180) {
		return -1;
	}
	if (outer_box[1].y < 0 &&
		(0 <= car->car.direction && car->car.direction <= 90 ||
			270 <= car->car.direction && car->car.direction <= 360)) {
		return -1;
	}
	if (outer_box[0].y > car->car.bg_h &&
		90 <= car->car.direction && car->car.direction <= 270) {
		return -1;
	}
	return 0;
}

static VECTOR normalize(VECTOR v) {
	return VGet(v.y, -v.x, 0);
}

static int intersect(GraphBBCar self, GraphBBCar other) {
	VECTOR* me_outer = self->bbc.outer_box;
	VECTOR* you_outer = other->bbc.outer_box;

	// 外接矩形同士の当たり判定
	if ((you_outer[1].x < me_outer[0].x || me_outer[1].x < you_outer[0].x) &&
		(you_outer[1].y < me_outer[0].y || me_outer[1].y < you_outer[0].y)) {
		return 0;
	}

	// 内接矩形同士の当たり判定 SATで判定
	VECTOR* me_inner = self->bbc.inner_box;
	VECTOR* you_inner = other->bbc.inner_box;

	VECTOR norm[2][4];
	for (int i = 0; i < 4; i++) {
		norm[0][i] = normalize(VSub(me_inner[(i + 1) % 4], me_inner[i]));
		norm[1][i] = normalize(VSub(you_inner[(i + 1) % 4], you_inner[i]));
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

void collide(GraphBase self, GraphManager manager) {
	GraphBBCar bbc = (GraphBBCar)self;
	GraphNode me = manager->gman.graph_to_node(manager, self);
	if (me == NULL)
		return;

	GraphNode you = manager->gman.get_next(manager, me);
	GraphBBCar you_car;
	while (you != NULL) {
		you_car = (GraphBBCar)you->gnode.get_graph(you);
		if (intersect(bbc, you_car)) {
			// 自車から相手へのベクトル
			VECTOR forward = VSub(you_car->base.coordinates, bbc->base.coordinates);
			// 衝突面のベクトル
			VECTOR v, m[2], y[2]; float min = 1e9;
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++) {
					m[0] = bbc->bbc.inner_box[i];
					m[1] = bbc->bbc.inner_box[(i + 1) % 4];
					y[0] = you_car->bbc.inner_box[j];
					y[1] = you_car->bbc.inner_box[(j + 1) % 4];
					float tmp = Segment_Segment_MinLength(m[0],m[1],y[0],y[1]);
					if (tmp < min) {
						min = tmp;
						v = VSub(y[1], y[0]);
					}
				}
			}
			float cos_sim = VDot(forward, v);
			if (cos_sim < 0) {
				v = VGet(-v.x, -v.y, 0);
			}
			// 衝突面の法線ベクトル
			VECTOR n = VGet(-v.y, v.x, 0);
			// 反射ベクトルを求める
			float a = VDot(VGet(-forward.x, -forward.y, 0), n);
			// reflect = forward + 2 * a * n
			VECTOR reflect = VAdd(forward, VGet(n.x * 2, n.y * 2, n.z));
			/*DrawLine(
				bbc->base.coordinates.x + forward.x,
				bbc->base.coordinates.y + forward.y,
				bbc->base.coordinates.x + reflect.x,
				bbc->base.coordinates.y + reflect.y,
				GetColor(127, 38, 118), 3);
			DrawLine(
				you_car->base.coordinates.x,
				you_car->base.coordinates.y,
				you_car->base.coordinates.x + v.x,
				you_car->base.coordinates.y + v.y,
				GetColor(56, 74, 192), 3);*/
			// 反射ベクトルの角度を求める
			double deg = atan2(reflect.y, reflect.x) * 180 / M_PI + 180;
			bbc->car.direction = deg;

			// ふっとび
			int imp = max(0, bbc->bbc.weight - you_car->bbc.weight);
			imp -= bbc->car.speed > you_car->car.speed ? 2 : 0;
			int adjust_me = bbc->bbc.weight + imp;
			int adjust_you = max(1, you_car->bbc.weight - imp / 4);
			bbc->base.coordinates = VAdd(bbc->base.coordinates, VGet(-forward.x / adjust_me, -forward.y / adjust_me, 0));
			you_car->base.coordinates = VAdd(you_car->base.coordinates, VGet(forward.x / adjust_you, forward.y / adjust_you, 0));

			bbc->car.direction += imp * cos_sim;
		}
		you = manager->gman.get_next(manager, you);
	}
}