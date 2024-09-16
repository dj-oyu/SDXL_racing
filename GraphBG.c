#include <stdio.h>
#include "DxLib.h"
#include "GraphBGP.h"

static void init(Core p);
static void update_bg(GraphBase p);
static void draw_bg(GraphBase p);

GraphBGClassDescriptor graphBG_class_descriptor = {
	/* Core part */
	{
		"GraphBG",                    /* class_name */
		(CoreClassDescriptor*)&graph_base_class_descriptor,  /* super_class */
		sizeof(GraphBGObj),           /* size_of_instance */
		NULL,                         /* class_initializer */
		init,                         /* initializer */
		NULL,						 /* finalizer */
	},
	/* GraphBase part */
	{
		update_bg,                    /* update */
		draw_bg,                      /* draw */
	},
	/* GraphBG part */
	{
		0,							  /* height */
		0,                            /* imagePath */
	},
};

CoreClassDescriptor* graphBGClass = (CoreClassDescriptor*)&graphBG_class_descriptor;

CoreClassDescriptor* setupBG(int height, const char* imagePath) {
	(*(GraphBGClassDescriptor*)graphBGClass).bg.height = height;
	(*(GraphBGClassDescriptor*)graphBGClass).bg.imagePath = imagePath;

	return graphBGClass;
}

static void init(Core p) {
	GraphBG bg = (GraphBG)p;

	bg->base.image = LoadGraph(bg->bg.imagePath, FALSE);

	printf("GraphBG::init\n");
}

static void update_bg(GraphBase p) {
	GraphBG bg = (GraphBG)p;
	int h = bg->bg.height;

	bg->base.y += 10;
	if (bg->base.y >= h) {
		bg->base.y -= h;
	}
	printf("GraphBG::update\n");
}

static void draw_bg(GraphBase p) {
	GraphBG bg = (GraphBG)p;
	int h = bg->bg.height;

	// upper side
	DrawGraph(bg->base.x, bg->base.y - h, bg->base.image, FALSE);
	// lower side
	DrawGraph(bg->base.x, bg->base.y, bg->base.image, FALSE);

	printf("GraphBG::draw\n");
}