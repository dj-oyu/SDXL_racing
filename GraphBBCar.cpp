#include <math.h>
#include "DxLib.h"
#include "framework.h"
#include "GraphNodeP.h"
#include "GraphBBCarP.h"

#define DEFAULT_LIFETIME 90
const float LOWEST_SPEED = 1e-5f;

static void init(Core p);
static void fin(Core p);
static int update_car(GraphBase p);
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
	GraphBBCar bbc = (GraphBBCar)p;

	if (fabs(bbc->car.speed) <= LOWEST_SPEED) {
		bbc->bbc.lifetime--;
	}
	if (bbc->bbc.lifetime <= 0) {
		return -1;
	}

	if (rand() % 20 == 0)
		bbc->car.rotate((GraphCar)bbc, rand() % 100 - 50);

	double s = bbc->car.speed;
	double rad = (bbc->car.direction) * M_PI / 180;
	VECTOR move = VGet(sin(rad) * s, -cos(rad) * s, 0);

	bbc->base.coordinates = VAdd(bbc->base.coordinates, move);

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
		180 <= bbc->car.direction && bbc->car.direction <= 360) {
		return -1;
	}
	if (outer_box[0].x > bbc->car.bg_w &&
		0 <= bbc->car.direction && bbc->car.direction <= 180) {
		return -1;
	}
	if (outer_box[1].y < 0 &&
		(0 <= bbc->car.direction && bbc->car.direction <= 90 ||
			270 <= bbc->car.direction && bbc->car.direction <= 360)) {
		return -1;
	}
	if (outer_box[0].y > bbc->car.bg_h &&
		90 <= bbc->car.direction && bbc->car.direction <= 270) {
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
		if (you_car->core.class_descriptor != graphBBCarClass) {
			you = manager->gman.get_next(manager, you);
			continue;
		}

		if (intersect(bbc, you_car)) {
			// 衝突点へのベクトル
			VECTOR cross_vec, // 衝突点へのベクトル (ワールド座標)
				hit_surface; // 衝突点から当たり判定エンドへのベクトル (相対座標)
			VECTOR boundary_line[2]; // 相手側の当たり判定
			HITRESULT_LINE test;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					test = HitCheck_Line_Triangle(
						bbc->bbc.inner_box[i],
						bbc->bbc.inner_box[(i + 1) % 4],
						you_car->base.coordinates,
						you_car->bbc.inner_box[j],
						you_car->bbc.inner_box[(j + 1) % 4]);
					if (test.HitFlag) {
						cross_vec = test.Position;
						hit_surface = VSub(you_car->bbc.inner_box[(j + 1) % 4], test.Position);

						boundary_line[0] = you_car->bbc.inner_box[j];
						boundary_line[1] = you_car->bbc.inner_box[(j + 1) % 4];
						break;
					}
				}
			}
			// 自車中心から衝突点へのベクトル (相対座標)
			VECTOR forward = VSub(cross_vec, bbc->base.coordinates);
			float cos_sim = VDot(forward, hit_surface) / (VSize(forward) * VSize(hit_surface));
			if (cos_sim < 0) {
				hit_surface = VGet(-hit_surface.x, -hit_surface.y, 0);
			}
			// 衝突面の法線ベクトル
			VECTOR n = VNorm(VGet(-hit_surface.y, hit_surface.x, 0));
			// 反射ベクトルを求める係数
			float a = VDot(VGet(-forward.x, -forward.y, 0), n);
			// 反射ベクトル
			// reflect = forward + 2 * a * n
			VECTOR reflect = VAdd(forward, VGet(2 * a * n.x, 2 * a * n.y, n.z));
			DrawLine(
				bbc->base.coordinates.x + forward.x,
				bbc->base.coordinates.y + forward.y,
				bbc->base.coordinates.x + forward.x + reflect.x,
				bbc->base.coordinates.y + forward.y + reflect.y,
				GetColor(255, 255, 255), 3);
			DrawLine(
				boundary_line[0].x,
				boundary_line[0].y,
				boundary_line[1].x,
				boundary_line[1].y,
				GetColor(0, 255, 0), 3);
			// 反射ベクトルの角度を求める
			double deg = atan2(reflect.y, reflect.x) * 180 / M_PI + 180;
			bbc->car.direction = deg;

			// ふっとび
			bbc->base.coordinates = VAdd(bbc->base.coordinates, VScale(reflect, 10 / bbc->bbc.weight));
		}
		you = manager->gman.get_next(manager, you);
	}
}