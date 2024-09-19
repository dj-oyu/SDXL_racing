#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "GraphManagerP.h"
#include "GraphNodeP.h"
#include "framework.h"

/* Private Field */
struct GraphNode_private {
	GraphNode top;
	GraphNode last;
	GraphNode current;
};

static GraphManager GraphManager_new() {
	GraphManager g = (GraphManager)new_instance(graphManagerClass);
	return g;
}

static void init(Core p);
static void fin(Core p);
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
		(CoreClassDescriptor*)&core_class_descriptor,  /* super_class */
		sizeof(GraphManagerObj),           /* size_of_instance */
		NULL,                              /* class_initializer */
		init,                          /* initializer */
		fin,						   /* finalizer */
	},
	/* GraphManager part */
	{
		GraphManager_new,                 /* constructor */
	},
};

CoreClassDescriptor* graphManagerClass = (CoreClassDescriptor*)&graphManager_class_descriptor;

/* functions */
static void init(Core p) {
	GraphManager g = (GraphManager)p;
	g->gman.p = (GraphNode_private*)malloc(sizeof(GraphNode_private));
	g->gman.p->top = NULL;
	g->gman.p->last = NULL;
	g->gman.p->current = NULL;

	g->gman.len = len;
	g->gman.add_node = add_node;
	g->gman.remove_node = remove_node;
	g->gman.hasNext = hasNext;
	g->gman.next = next;
	g->gman.render_nodes = render_nodes;
}

static void fin(Core p) {
	GraphManager g = (GraphManager)p;
	GraphNodeClassDescriptor* node_clazz = (GraphNodeClassDescriptor*)graphNodeClass;

	GraphNode last = g->gman.p->last;
	GraphNode tmp;
	do {
		tmp = last->gnode.prev;
		node_clazz->core.finalizer((Core)last);
	} while ((last = tmp) != NULL);

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
	GraphNode node = ((GraphNodeClassDescriptor*)graphNodeClass)->gnode.constructor(graph);

	if (self->gman.p->top == NULL) {
		self->gman.p->top = node;
		self->gman.p->current = node;
	}
	else {
		self->gman.p->last->gnode.next = node;
		node->gnode.prev = self->gman.p->last;
	}
	self->gman.p->last = node;
}

static void remove_node(GraphManager self, GraphNode node) {
	if (self->gman.p->top == node) {
		self->gman.p->top = node->gnode.next;
	}
	if (self->gman.p->last == node) {
		self->gman.p->last = node->gnode.prev;
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
	GraphNodeClassDescriptor* node_clazz = (GraphNodeClassDescriptor*)node->core.class_descriptor;
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
	return (self->gman.p->current = self->gman.p->current->gnode.next);
}

static void render_nodes(GraphManager self) {
	GraphNode current = (self->gman.p->current = self->gman.p->top);
	if (current == NULL) {
		return;
	}

	GraphBase object;
	do {
		object = current->gnode.get_graph(current);
		object->base.draw(object);
		if (object->base.update(object) != 0) {
			self->gman.remove_node(self, current);
		}
		current = self->gman.next(self);
	} while (self->gman.hasNext(self));

	self->gman.p->current = self->gman.p->top;
}
