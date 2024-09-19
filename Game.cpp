#include <stdio.h>
#include <time.h>
#include "DxLib.h"
#include "Game.h"
#include "framework.h"
#include "GraphBase.h"
#include "GraphBG.h"
#include "GraphCar.h"
#include "GraphManager.h"
#include "GraphManagerP.h"

GraphBase spawnCar(const char* path) {
	GraphCar c = (GraphCar)new_instance(graphCarClass);
	setupCar(c,
		rand() % WIDTH, rand() % HEIGHT,
		LoadGraph(path),
		WIDTH, HEIGHT, 
		rand()%4+1, rand() % 360);

	return (GraphBase)c;
}

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

	GraphManager gman = (GraphManager)new_instance(graphManagerClass);
	GraphManagerClassDescriptor* gman_clazz = (GraphManagerClassDescriptor*)graphManagerClass;

	GraphBG bg = (GraphBG)new_instance(graphBGClass);
	setupBG(bg, LoadGraph(BG_IMAGE_PATH), HEIGHT);
	gman_clazz->gman.add_node(gman, (GraphBase)bg);

	while (1) {
		ClearDrawScreen();
		while (gman_clazz->gman.len(gman) < 5) {
			gman_clazz->gman.add_node(gman, spawnCar(car_image_path[rand() % 4]));
		}
		gman_clazz->gman.render_nodes(gman);

		ScreenFlip();
		WaitTimer(16);
		if (ProcessMessage() == -1) break;
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break;
	}

	DxLib_End();
	return 0;
}