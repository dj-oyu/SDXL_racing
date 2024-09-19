#pragma once
#include "CoreP.h"
#include "GraphBaseP.h"
#include "GraphCacheAdapter.h"

typedef struct {
	GraphCacheAdapter(*constructor)(void);
} GraphCacheAdapterClassDescriptorPart;

struct  GraphCacheAdapterClassDescriptor_tag {
	CoreClassDescriptorPart core;
	GraphBaseClassDescriptorPart base;
	GraphCacheAdapterClassDescriptorPart gcache;
};

struct Handle_Cache;
typedef struct {
	struct Handle_Cache* cache;
	GraphBase(*create_graph)(GraphCacheAdapter self, GraphBaseClassDescriptor* clazz, const char* path, ... );
} GraphCacheAdapterPart;

typedef struct GraphCacheAdapterObj_tag {
	CorePart core;
	GraphBasePart base;
	GraphCacheAdapterPart gcache;
} GraphCacheAdapterObj;