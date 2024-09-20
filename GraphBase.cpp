#include <stdio.h>
#include "DxLib.h"
#include "GraphBaseP.h"

static void init(Core p);

GraphBaseClassDescriptor graph_base_class_descriptor = {
	/* Core part */
	{
		"GraphBase",             /* class_name */
		&core_class_descriptor,  /* super_class */
		sizeof(GraphBaseObj),    /* size_of_instance */
		NULL,                    /* class_initializer */
		init,                    /* initializer */
		NULL,                    /* finalizer */
	},
	/* GraphBase part */
	{
		NULL,                    /* constructor */
	},
};

CoreClassDescriptor* graphBaseClass = (CoreClassDescriptor*)&graph_base_class_descriptor;

static void init(Core p) {
	GraphBase base = (GraphBase)p;

	base->base.coordinates = VGet(0, 0, 0);
	base->base.image = 0;
	base->base.update = NULL;
	base->base.draw = NULL;
}
