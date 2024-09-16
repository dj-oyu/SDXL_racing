#include "DxLib.h"
#include "Game.h"
#include "framework.h"
#include "GraphBase.h"
#include "GraphBG.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	SetWindowText("カーレース");
	SetGraphMode(WIDTH, HEIGHT, 32, 60);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1) return -1;
	SetBackgroundColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);

	CoreClassDescriptor* bg_descriptor = setupBG(HEIGHT, "image/bg.png");
	GraphBG bg = (GraphBG)new_instance(bg_descriptor);
	GraphBase g = (GraphBase)bg;

	while (1) {
		ClearDrawScreen();
		update(g);
		draw(g);
		ScreenFlip();
		WaitTimer(16);
		if (ProcessMessage() == -1) break;
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break;
	}

	DxLib_End();
	return 0;
}