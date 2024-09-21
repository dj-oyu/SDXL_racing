#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "GraphManagerP.h"
#include "GraphNodeP.h"
#include "framework.h"
#include "GraphBBCarP.h"

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
static GraphNode graph_to_node(GraphManager self, GraphBase graph);
static GraphNode get_next(GraphManager self, GraphNode node);
static void add_node(GraphManager self, GraphBase graph);
static void remove_node(GraphManager self, GraphNode node);
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
	g->gman.graph_to_node = graph_to_node;
	g->gman.get_next = get_next;
	g->gman.add_node = add_node;
	g->gman.remove_node = remove_node;
	g->gman.render_nodes = render_nodes;
}

static void fin(Core p) {
	GraphManager g = (GraphManager)p;
	void (*f)(Core);

	GraphNode last = g->gman.p->last;
	GraphNode tmp;
	do {
		tmp = last->gnode.prev;
		f = last->core.class_descriptor->core.finalizer;
		f((Core)last);
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

static GraphNode graph_to_node(GraphManager self, GraphBase graph) {
	GraphNode node = self->gman.p->top;
	while (node->gnode.get_graph(node) != graph) {
		if (node == NULL) {
			return NULL;
		}
		node = node->gnode.next;
	}
	return node;
}

static GraphNode get_next(GraphManager self, GraphNode node) {
	GraphNode next = self->gman.p->top;
	while (next != node) {
		if (next == NULL) {
			return NULL;
		}
		next = next->gnode.next;
	}
	return next->gnode.next;
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
	void(*f)(Core) = node->core.class_descriptor->core.finalizer;
	f((Core)node);
}

static void render_nodes(GraphManager self) {
	GraphNode current = (self->gman.p->current = self->gman.p->top);
	GraphBase object;
	while (current != NULL) {
		object = current->gnode.get_graph(current);

		if (object->base.update(object) != 0) {
			GraphNode prev = current;
			current = current->gnode.next;
			self->gman.remove_node(self, prev);
			continue;
		}
		object->base.draw(object);
		current = current->gnode.next;
	}

	current = self->gman.p->top;
	while (current != NULL) {
		object = current->gnode.get_graph(current);
		object->base.finish_draw(object, self);
		current = current->gnode.next;
	}
}
