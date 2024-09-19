#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "GraphManagerP.h"
#include "GraphNodeP.h"
#include "framework.h"

/* Private Field */
struct GraphNode_private {
	GraphNode top;
	GraphNode current;
};

static void init_man(Core p);
static void fin_man(Core p);
static int len(GraphManager self);
static void add_node(GraphManager self, GraphBase graph);
static void remove_node(GraphManager self, GraphNode node);
static int hasNext(GraphManager self);
static GraphNode next(GraphManager self);
static void render_nodes(GraphManager self);

GraphManagerClassDescriptor graphManager_class_descriptor = {
	/* Core part */
	{
		"GraphManager",                    /* class_name */
		(CoreClassDescriptor*)&graph_base_class_descriptor,  /* super_class */
		sizeof(GraphManagerObj),           /* size_of_instance */
		NULL,                              /* class_initializer */
		init_man,                          /* initializer */
		fin_man,						       /* finalizer */
	},
	/* GraphManager part */
	{
		len,                               /* len */
		add_node,                          /* add_node */
		remove_node,                       /* remove_node */
		hasNext,                           /* hasNext */
		next,                              /* next */
		render_nodes,                      /* render_nodes */
	},
};

CoreClassDescriptor* graphManagerClass = (CoreClassDescriptor*)&graphManager_class_descriptor;

/* functions */
static void init_man(Core p) {
	GraphManager g = (GraphManager)p;
	g->gman.p = (GraphNode_private*)malloc(sizeof(GraphNode_private));
	g->gman.p->top = NULL;
	g->gman.p->current = NULL;
}

static void fin_man(Core p) {
	GraphManager g = (GraphManager)p;
	GraphManagerClassDescriptor* clazz = (GraphManagerClassDescriptor*)graphManagerClass;

	GraphNode last = g->gman.p->top;
	while (clazz->gman.hasNext(g)) {
		last = clazz->gman.next(g);
	}

	GraphNodeClassDescriptor* node_clazz = (GraphNodeClassDescriptor*)graphNodeClass;
	GraphNode tmp;
	while (last != NULL) {
		tmp = last->gnode.prev;
		node_clazz->core.finalizer((Core)last);
		last = tmp;
	}
	free(g->gman.p);
	free(g);
	p = NULL;
}

static int len(GraphManager self) {
	int len = 0;
	GraphNode last = self->gman.p->top;
	while (last != NULL) {
		len++;
		last = last->gnode.next;
	}
	return len;
}

static void add_node(GraphManager self, GraphBase graph) {
	GraphNode node = (GraphNode)new_instance(graphNodeClass);
	setGraphNode(node, graph);

	if (self->gman.p->top == NULL) {
		self->gman.p->top = node;
		self->gman.p->current = node;
	}
	else {
		GraphNode last = self->gman.p->top;
		while (last->gnode.next != NULL) {
			last = last->gnode.next;
		}
		last->gnode.next = node;
		node->gnode.prev = last;
	}
}

static void remove_node(GraphManager self, GraphNode node) {
	if (self->gman.p->top == node) {
		self->gman.p->top = node->gnode.next;
	}
	if (self->gman.p->current == node) {
		self->gman.p->current = node->gnode.prev;
	}
	if (node->gnode.prev != NULL) {
		node->gnode.prev->gnode.next = node->gnode.next;
	}
	if (node->gnode.next != NULL) {
		node->gnode.next->gnode.prev = node->gnode.prev;
	}
	GraphNodeClassDescriptor* node_clazz = (GraphNodeClassDescriptor*)graphNodeClass;
	node_clazz->core.finalizer((Core)node);
}

static int hasNext(GraphManager self) {
	if (self->gman.p->top == NULL ||
		self->gman.p->current == NULL) {
		return 0;
	}
	return self->gman.p->current->gnode.next == NULL ? 0 : 1;
}

static GraphNode next(GraphManager self) {
	if (self->gman.p->current == NULL) {
		return NULL;
	}
	self->gman.p->current = self->gman.p->current->gnode.next;
	return self->gman.p->current;
}

static void render_nodes(GraphManager self) {
	GraphManagerClassDescriptor* clazz = (GraphManagerClassDescriptor*)graphManagerClass;
	GraphNodeClassDescriptor* node_clazz = (GraphNodeClassDescriptor*)graphNodeClass;
	GraphBaseClassDescriptor* base_clazz;

	self->gman.p->current = self->gman.p->top;
	do {
		base_clazz = (GraphBaseClassDescriptor*)(node_clazz->gnode.get_graph(
			(GraphNode)self->gman.p->current
		)->core.class_descriptor);

		base_clazz->base.draw(
			(GraphBase)node_clazz->gnode.get_graph(
				(GraphNode)self->gman.p->current
			)
		);
		if (base_clazz->base.update(
			(GraphBase)node_clazz->gnode.get_graph(
				(GraphNode)self->gman.p->current
			)
		) != 0) {
			clazz->gman.remove_node(self, self->gman.p->current);
		}
		clazz->gman.next(self);
	} while (clazz->gman.hasNext(self));

	self->gman.p->current = self->gman.p->top;
}
