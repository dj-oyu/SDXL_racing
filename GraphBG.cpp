#include <stdio.h>
#include "DxLib.h"
#include "framework.h"
#include "GraphBGP.h"

static void init(Core p);
static void fin(Core p);
static int update_bg(GraphBase p);
static int draw_bg(GraphBase p);

static GraphBG GraphBG_new(int image, int bg_height) {
	GraphBG bg = (GraphBG)new_instance(graphBGClass);
	bg->base.image = image;
	bg->bg.bg_height = bg_height;
	return bg;
}

static GraphBase trampoline_constructor(int nargs, ... ) {
	if (nargs != 2) {
		return NULL;
	}
	va_list ap;
	va_start(ap, nargs);

	int image = va_arg(ap, int);
	int bg_height = va_arg(ap, int);
	va_end(ap);

	return (GraphBase)GraphBG_new(image, bg_height);
}

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
		trampoline_constructor,		  /* constructor */
	},
	/* GraphBG part */
	{
		GraphBG_new,				  /* constructor */
	},
};

CoreClassDescriptor* graphBGClass = (CoreClassDescriptor*)&graphBG_class_descriptor;

static void init(Core p) {
	GraphBG bg = (GraphBG)p;
	bg->base.draw = draw_bg;
	bg->base.update = update_bg;
	bg->bg.bg_height = 0;
}

static void fin(Core p) {
	GraphBG bg = (GraphBG)p;

	DeleteGraph(bg->base.image);
	free(bg);
	p = NULL;
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