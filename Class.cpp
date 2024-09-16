#include "DxLib.h"
#include "Class.h"

GraphicManager* newGraphicManager(Graphic* current) {
	GraphicManager* g = (GraphicManager*)malloc(sizeof(GraphicManager));
	g->g = current;
	g->next = NULL;
	g->add = addGraphic;
	return g;
}

void addGraphic(GraphicManager* self, Graphic* g) {
	GraphicManager* top = self;
	while (top->next != NULL) {
		top = top->next;
	}
	top->next = newGraphicManager(g);
}

void update_bg(Graph_BG* self) {
	self->y += 5;
	if (self->y >= self->height)
		self->y = self->y - self->height;
}

void draw_bg(Graph_BG* self) {
	DrawGraph(0, self->y, self->Img, FALSE);
	DrawGraph(0, self->y - self->height, self->Img, FALSE);
}

void loadGraphic(Graphic* g, ...) {
	va_list args;
	va_start(args, g);

	switch (g->type) {
	case BG:
		g->graph.bg.y = 0;
		g->graph.bg.height = va_arg(args, int);
		g->graph.bg.Img = LoadGraph(va_arg(args, char*));
		g->graph.bg.update = update_bg;
		g->graph.bg.draw = draw_bg;
		break;
	}
	va_end(args);
}

Graphic* newBG(int height, const char* path) {
	Graphic* g = (Graphic*)malloc(sizeof Graphic);
	g->type = BG;
	loadGraphic(g, height, path);
	return g;
};
