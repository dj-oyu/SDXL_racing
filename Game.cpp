#include <stdio.h>
#include <time.h>
#include "DxLib.h"
#include "Game.h"
#include "framework.h"
#include "GraphBase.h"
#include "GraphBG.h"
#include "GraphCar.h"
#include "GraphCarP.h"

void spawnRandom(GraphCar self, const char* path) {
	setupCar(self,
		rand() % WIDTH, rand() % HEIGHT,
		LoadGraph(path),
		WIDTH, HEIGHT, 
		rand()%4+1, rand() % 360);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	
	srand((unsigned int)time(NULL));
	const char* BG_IMAGE_PATH = "image/bg.png";

	SetWindowText("カーレース");
	SetGraphMode(WIDTH, HEIGHT, 32, 60);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1) return -1;
	SetBackgroundColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);

	GraphBG bg = (GraphBG)new_instance(graphBGClass);
	setupBG(bg, LoadGraph(BG_IMAGE_PATH), HEIGHT);

	GraphBase g = (GraphBase)bg;

	const char* car_image_path[] = {
		"image/car_blue.png",
		"image/car_red.png",
		"image/car_yellow.png",
		"image/truck.png",
	};
	GraphBase cars[] = {
		(GraphBase)new_instance(graphCarClass),
		(GraphBase)new_instance(graphCarClass),
		(GraphBase)new_instance(graphCarClass),
	};
	spawnRandom((GraphCar)cars[0], car_image_path[rand() % 4]);
	spawnRandom((GraphCar)cars[1], car_image_path[rand() % 4]);
	spawnRandom((GraphCar)cars[2], car_image_path[rand() % 4]);

	while (1) {
		ClearDrawScreen();
		update(g);
		draw(g);

		for (int i = 0; i < 3; i++) {
			draw(cars[i]);
			if (update(cars[i]) != 0) {
				cars[i] = (GraphBase)new_instance(graphCarClass);
				spawnRandom((GraphCar)cars[i], car_image_path[rand() % 4]);
			}
		}

		ScreenFlip();
		WaitTimer(16);
		if (ProcessMessage() == -1) break;
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break;
	}

	DxLib_End();
	return 0;
}