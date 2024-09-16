#pragma once

typedef enum {
	BG, CAR
} ImgType;

typedef struct Graph_BG_tag {
	int y, height;
	int Img;
	void (*update)(Graph_BG_tag*);
	void (*draw)(Graph_BG_tag*);
} Graph_BG;

typedef struct Graph_Car_tag {
	int x, y;
	int Img;
	void (*update)(Graph_Car_tag*);
	void (*draw)(Graph_Car_tag*);
} Graph_Car;

typedef struct {
	ImgType type;
	union {
		Graph_BG bg;
		Graph_Car car;
	} graph;
} Graphic;

typedef struct GraphicManager_tag {
	Graphic* g;
	GraphicManager_tag* next;
	void (*add)(GraphicManager_tag*, Graphic*);
} GraphicManager;

void addGraphic(GraphicManager*, Graphic*);

Graphic* newBG(int height, const char* path);

GraphicManager* newGraphicManager(Graphic* current);
