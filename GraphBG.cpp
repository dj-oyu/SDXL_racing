#include <stdio.h>
#include "DxLib.h"
#include "GraphBGP.h"

static void init(Core p);
static void fin(Core p);
static int update_bg(GraphBase p);
static int draw_bg(GraphBase p);
static void set_img(GraphBG self, int image, int bg_height);

GraphBGClassDescriptor graphBG_class_descriptor = {
	/* Core part */
	{
		"GraphBG",                    /* class_name */
		(CoreClassDescriptor*)&graph_base_class_descriptor,  /* super_class */
		sizeof(GraphBGObj),           /* size_of_instance */
		NULL,                         /* class_initializer */
		init,                         /* initializer */
		fin,						  /* finalizer */
	},
	/* GraphBase part */
	{
		0,							  /* dummy */
	},
	/* GraphBG part */
	{
		0,							  /* dummy */
	},
};

CoreClassDescriptor* graphBGClass = (CoreClassDescriptor*)&graphBG_class_descriptor;

static void init(Core p) {
	GraphBG bg = (GraphBG)p;
	bg->base.draw = draw_bg;
	bg->base.update = update_bg;
	bg->bg.bg_height = 0;
	bg->bg.set_img = set_img;
}

static void fin(Core p) {
	GraphBG bg = (GraphBG)p;

	DeleteGraph(bg->base.image);
	free(bg);
	p = NULL;
}

static void set_img(GraphBG self, int image, int bg_height) {
	self->base.image = image;
	self->bg.bg_height = bg_height;
}

static int update_bg(GraphBase p) {
	GraphBG bg = (GraphBG)p;
	int h = bg->bg.bg_height;

	bg->base.y += 10;
	if (bg->base.y >= h) {
		bg->base.y -= h;
	}
	return 0;
}

static int draw_bg(GraphBase p) {
	GraphBG bg = (GraphBG)p;
	int h = bg->bg.bg_height;
	int result = 0;
	// upper side
	result |= DrawGraph(bg->base.x, bg->base.y - h, bg->base.image, FALSE);
	// lower side
	result |= DrawGraph(bg->base.x, bg->base.y, bg->base.image, FALSE);

	return result;
}