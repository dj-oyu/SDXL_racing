#include <stdio.h>
#include "DxLib.h"
#include "GraphBGP.h"

static void init(Core p);
static int update_bg(GraphBase p);
static int draw_bg(GraphBase p);

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
		0,							  /* dummy */
	},
};

CoreClassDescriptor* graphBGClass = (CoreClassDescriptor*)&graphBG_class_descriptor;

void setupBG(GraphBG self, int image, int height) {
	self->base.image = image;
	self->bg.height = height;
}

static void init(Core p) {
	GraphBG bg = (GraphBG)p;

	bg->bg.height = 0;

	printf("GraphBG::init\n");
}

static int update_bg(GraphBase p) {
	GraphBG bg = (GraphBG)p;
	int h = bg->bg.height;

	bg->base.y += 10;
	if (bg->base.y >= h) {
		bg->base.y -= h;
	}
	return 0;
}

static int draw_bg(GraphBase p) {
	GraphBG bg = (GraphBG)p;
	int h = bg->bg.height;
	int result = 0;
	// upper side
	result |= DrawGraph(bg->base.x, bg->base.y - h, bg->base.image, FALSE);
	// lower side
	result |= DrawGraph(bg->base.x, bg->base.y, bg->base.image, FALSE);

	return result;
}