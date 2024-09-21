#include <stdio.h>
#include <time.h>
#include "DxLib.h"
#include "Game.h"
#include "GraphBase.h"
#include "GraphBGP.h"
#include "GraphCarP.h"
#include "GraphBBCarP.h"
#include "GraphManagerP.h"
#include "GraphCacheAdapterP.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	
	srand((unsigned int)time(NULL));
	const char* BG_IMAGE_PATH = "image/bg.png";
	const char* car_image_path[] = {
		"image/car_blue.png",
		"image/car_red.png",
		"image/car_yellow.png",
		"image/truck.png",
	};

	/* グラフィック生成 */
	GraphManager gman = ((GraphManagerClassDescriptor*)graphManagerClass)->gman.constructor();
	GraphCacheAdapter gcache = ((GraphCacheAdapterClassDescriptor*)graphCacheAdapterClass)->gcache.constructor();
	GraphBase(*spawn_g)(GraphCacheAdapter, GraphBaseClassDescriptor*, const char*, ...);
	spawn_g = gcache->gcache.create_graph;

	SetWindowText("カーレース");
	SetGraphMode(WIDTH, HEIGHT, 32, 60);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1) return -1;
	SetBackgroundColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);

	GraphBase bg = spawn_g(gcache, (GraphBaseClassDescriptor*)graphBGClass, BG_IMAGE_PATH, HEIGHT);
	gman->gman.add_node(gman, bg);

	while (1) {
		ClearDrawScreen();
		while (gman->gman.len(gman) < 15) {
			gman->gman.add_node(gman,
				spawn_g(gcache, (GraphBaseClassDescriptor*)graphBBCarClass,
					car_image_path[rand() % 4],      /* image path */
					rand() % (WIDTH / 3) + WIDTH / 3, rand() % HEIGHT, /* x, y */
					WIDTH, HEIGHT,
					rand() % 4 + 1, rand() % 360)    /* speed, direction */
			);
		}
		gman->gman.render_nodes(gman);

		ScreenFlip();
		WaitTimer(16);
		if (ProcessMessage() == -1) break;
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break;
	}

	DxLib_End();
	return 0;
}