#include <stdio.h>
#include "DxLib.h"
#include "framework.h"
#include "GraphBGP.h"

static void init(Core p);
static void fin(Core p);
static int update_bg(GraphBase p);
static int draw_bg(GraphBase p);

static void finish_nop(GraphBase p, GraphManager m) {}

static GraphBG GraphBG_new(int image, int bg_height) {
	GraphBG bg = (GraphBG)new_instance(graphBGClass);
	bg->base.image = image;
	bg->bg.bg_height = bg_height;
	return bg;
}

static GraphBase trampoline_constructor(int handle, va_list* ap) {
	va_list cpy;
	va_copy(cpy, *ap);
	int bg_height = va_arg(cpy, int);
	va_end(cpy);

	return (GraphBase)GraphBG_new(handle, bg_height);
}

GraphBGClassDescriptor graphBG_class_descriptor = {
	/* Core part */
	{
		"GraphBG",                      /* class_name */
		(CoreClassDescriptor*)&graph_base_class_descriptor,  /* super_class */
		sizeof(GraphBGObj),             /* size_of_instance */
		NULL,                           /* class_initializer */
		init,                           /* initializer */
		graphBaseClass->core.finalizer,	/* finalizer */
	},
	/* GraphBase part */
	{
		trampoline_constructor,		     /* constructor */
	},
	/* GraphBG part */
	{
		GraphBG_new,				     /* constructor */
	},
};

CoreClassDescriptor* graphBGClass = (CoreClassDescriptor*)&graphBG_class_descriptor;

static void init(Core p) {
	GraphBG bg = (GraphBG)p;
	bg->base.draw = draw_bg;
	bg->base.update = update_bg;
	bg->base.finish_draw = finish_nop;
	bg->bg.bg_height = 0;
}

static int update_bg(GraphBase p) {
	GraphBG bg = (GraphBG)p;
	int h = bg->bg.bg_height;

	bg->base.coordinates = VAdd(bg->base.coordinates, VGet(0, 10, 0));
	if (bg->base.coordinates.y >= h) {
		bg->base.coordinates.y -= h;
	}
	return 0;
}

static int draw_bg(GraphBase p) {
	GraphBG bg = (GraphBG)p;
	int h = bg->bg.bg_height;
	int result = 0;
	// upper side
	result |= DrawGraph(bg->base.coordinates.x, bg->base.coordinates.y - h, bg->base.image, FALSE);
	// lower side
	result |= DrawGraph(bg->base.coordinates.x, bg->base.coordinates.y, bg->base.image, FALSE);

	return result;
}