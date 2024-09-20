#include <stdio.h>
#include <stdlib.h>
#include "framework.h"
#include "GraphNodeP.h"

static void init(Core p);
static void fin(Core p);
static GraphBase get_graph(GraphNode self);

static GraphNode GraphNode_new(GraphBase graph) {
	GraphNode n = (GraphNode)new_instance(graphNodeClass);
	n->gnode.graph = graph;

	return n;
}

GraphNodeClassDescriptor graphNode_class_descriptor = {
	/* Core part */
	{
		"GraphNode",                     /* class_name */
		(CoreClassDescriptor*)&core_class_descriptor,  /* super_class */
		sizeof(GraphNodeObj),            /* size_of_instance */
		NULL,                            /* class_initializer */
		init,                            /* initializer */
		fin,						     /* finalizer */
	},
	/* GraphNode part */
	{
		GraphNode_new,                   /* constructor */
	},
};

CoreClassDescriptor* graphNodeClass = (CoreClassDescriptor*)&graphNode_class_descriptor;

/* GraphNode functions */
static void init(Core p) {
	GraphNode n = (GraphNode)p;
	n->gnode.graph = NULL;
	n->gnode.next = NULL;
	n->gnode.prev = NULL;
	n->gnode.get_graph = get_graph;
}

static void fin(Core p) {
	GraphNode n = (GraphNode)p;
	GraphBase g = n->gnode.graph;
	void(*f)(Core);
	if (g == NULL) {
		free(n);
		p = NULL;
		return;
	}
	f = g->core.class_descriptor->core.finalizer;
	f((Core)g);
	free(n);
	p = NULL;
}

static GraphBase get_graph(GraphNode self) {
	return self->gnode.graph;
}
