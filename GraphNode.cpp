#include <stdio.h>
#include <stdlib.h>
#include "GraphNodeP.h"

static void init_node(Core p);
static void fin_node(Core p);
static GraphBase get_graph(GraphNode self);
static void set_graph(GraphNode self, GraphBase graph);

GraphNodeClassDescriptor graphNode_class_descriptor = {
	/* Core part */
	{
		"GraphNode",                       /* class_name */
		(CoreClassDescriptor*)&core_class_descriptor,  /* super_class */
		sizeof(GraphNodeObj),              /* size_of_instance */
		NULL,                              /* class_initializer */
		init_node,                         /* initializer */
		fin_node,						   /* finalizer */
	},
	/* GraphNode part */
	{
		0,								   /* dummy */
	},
};

CoreClassDescriptor* graphNodeClass = (CoreClassDescriptor*)&graphNode_class_descriptor;

/* GraphNode functions */
static void init_node(Core p) {
	GraphNode n = (GraphNode)p;
	n->gnode.graph = NULL;
	n->gnode.next = NULL;
	n->gnode.prev = NULL;
	n->gnode.get_graph = get_graph;
	n->gnode.set_graph = set_graph;
}

static void fin_node(Core p) {
	GraphNode n = (GraphNode)p;
	free(n);
	p = NULL;
}

static GraphBase get_graph(GraphNode self) {
	return self->gnode.graph;
}

static void set_graph(GraphNode self, GraphBase graph) {
	self->gnode.graph = graph;
}
