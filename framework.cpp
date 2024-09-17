#include <stdio.h>
#include <stdlib.h>
#include "CoreP.h"

static void initialize_instance(CoreClassDescriptor* descriptor, Core p) {
	if (descriptor->core.super != NULL) {
		initialize_instance(descriptor->core.super, p);
	}
	if (descriptor->core.initializer != NULL) {
		descriptor->core.initializer(p);
	}
}

Core new_instance(CoreClassDescriptor* descriptor) {
	Core p = (Core)malloc(descriptor->core.size);
	if (p == NULL) {
		fprintf(stderr, "new_instance: malloc failed\n");
		exit(1);
	}
	p->core.class_descriptor = descriptor;
	initialize_instance(descriptor, p);
	return p;
}
