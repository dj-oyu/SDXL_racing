#include <stdio.h>
#include "GraphBaseP.h"

static void init(Core p);

GraphBaseClassDescriptor graph_base_class_descriptor = {
	/* Core part */
	{
		"GraphBase",            /* class_name */
		&core_class_descriptor, /* super_class */
		sizeof(GraphBaseObj),  /* size_of_instance */
		NULL,                   /* class_initializer */
		init,   /* initializer */
		NULL,                   /* finalizer */
	},
	/* GraphBase part */
	{
		NULL,                   /* update */
		NULL,                   /* draw */
	},
};

CoreClassDescriptor* graphBaseClass = (CoreClassDescriptor*)&graph_base_class_descriptor;

static void init(Core p) {
	GraphBase base = (GraphBase)p;

	base->base.x = 0;
	base->base.y = 0;
	base->base.image = 0;

	printf("GraphBase::init\n");
}

void update(GraphBase base) {
	GraphBaseClassDescriptor* class = (GraphBaseClassDescriptor*)base->core.class_descriptor;
	class->base.update(base);
}

void draw(GraphBase base) {
	GraphBaseClassDescriptor* class = (GraphBaseClassDescriptor*)base->core.class_descriptor;
	class->base.draw(base);
}
