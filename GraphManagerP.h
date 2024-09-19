#pragma once
#include "CoreP.h"
#include "GraphBaseP.h"
#include "GraphManager.h"
#include "GraphNode.h"

typedef struct {
	GraphManager(*constructor)();
} GraphManagerClassDescriptorPart;

struct GraphManagerClassDescriptor_tag {
	CoreClassDescriptorPart core;
	GraphManagerClassDescriptorPart gman;
};

struct GraphNode_private;
typedef struct {
	/* Private Field */
	GraphNode_private* p;
	int (*len)(GraphManager self);
	void (*add_node)(GraphManager self, GraphBase graph);
	void (*remove_node)(GraphManager self, GraphNode node);
	int (*hasNext)(GraphManager self);
	GraphNode(*next)(GraphManager self);
	void (*render_nodes)(GraphManager self);
} GraphManagerPart;

typedef struct GraphManagerObj_tag {
	CorePart core;
	GraphManagerPart gman;
} GraphManagerObj;
