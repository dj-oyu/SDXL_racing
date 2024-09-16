#include "DxLib.h"
#include "Class.h"

GraphicManager* gManager = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	const int WIDTH = 720, HEIGHT = 640;

	SetWindowText("カーレース");
	SetGraphMode(WIDTH, HEIGHT, 32);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1) return -1;
	SetBackgroundColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);

	Graphic* bg = newBG(HEIGHT, "image/bg.png");

	gManager = newGraphicManager(bg);

	while (1) {
		ClearDrawScreen();

		GraphicManager* top = gManager;
		while (top != NULL) {
			Graphic* g = top->g;
			switch (g->type) {
			case BG:
				g->graph.bg.update(&g->graph.bg);
				g->graph.bg.draw(&g->graph.bg);
				break;
			}
			top = top->next;
		}

		ScreenFlip();
		WaitTimer(16);
		if (ProcessMessage() == -1) break;
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break;
	}

	DxLib_End();
	return 0;
}