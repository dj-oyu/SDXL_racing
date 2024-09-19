#pragma once
#include "Core.h"
#include "GraphBaseP.h"
#include "GraphNode.h"

typedef struct {
	GraphNode(*constructor)(GraphBase graph);
} GraphNodeClassDescriptorPart;

struct GraphNodeClassDescriptor_tag {
	CoreClassDescriptorPart core;
	GraphNodeClassDescriptorPart gnode;
};

typedef struct {
	GraphBase graph;
	GraphNode next;
	GraphNode prev;
	GraphBase(*get_graph)(GraphNode self);
} GraphNodePart;

typedef struct GraphNodeObj_tag {
	CorePart core;
	GraphNodePart gnode;
} GraphNodeObj;
