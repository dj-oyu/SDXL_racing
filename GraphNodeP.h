#pragma once
#include "Core.h"
#include "GraphBaseP.h"
#include "GraphNode.h"


typedef struct {
	GraphBase(*get_graph)(GraphNode self);
} GraphNodeClassDescriptorPart;

struct GraphNodeClassDescriptor_tag {
	CoreClassDescriptorPart core;
	GraphNodeClassDescriptorPart gnode;
};

/* プライベートにする方法を調べる */
typedef struct {
	GraphBase graph;
	GraphNode next;
	GraphNode prev;
} GraphNodePart;

typedef struct GraphNodeObj_tag {
	CorePart core;
	GraphNodePart gnode;
} GraphNodeObj;

GraphBase setGraphNode(GraphNode self, GraphBase graph);
