#include <stdio.h>
#include <time.h>
#include "DxLib.h"
#include "Game.h"
#include "GraphBase.h"
#include "GraphBGP.h"
#include "GraphCarP.h"
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

	SetWindowText("カーレース");
	SetGraphMode(WIDTH, HEIGHT, 32, 60);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1) return -1;
	SetBackgroundColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);

	GraphManager gman = ((GraphManagerClassDescriptor*)graphManagerClass)->gman.constructor();
	GraphCacheAdapter gcache = ((GraphCacheAdapterClassDescriptor*)graphCacheAdapterClass)->gcache.constructor();
	// GraphBG bg = ((GraphBGClassDescriptor*)graphBGClass)->bg.constructor(LoadGraph(BG_IMAGE_PATH), HEIGHT);
	GraphBase bg = gcache->gcache.create_graph(gcache, (GraphBaseClassDescriptor*)graphBGClass, BG_IMAGE_PATH, HEIGHT);

	gman->gman.add_node(gman, (GraphBase)bg);

	while (1) {
		ClearDrawScreen();
		while (gman->gman.len(gman) < 50) {
			gman->gman.add_node(gman,
				gcache->gcache.create_graph(gcache, (GraphBaseClassDescriptor*)graphCarClass,
					car_image_path[rand() % 4],
					rand() % (WIDTH / 3) + WIDTH / 3, rand() % HEIGHT,
					WIDTH, HEIGHT,
					rand() % 4 + 1, rand() % 360)
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