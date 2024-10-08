#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DxLib.h"
#include "framework.h"
#include "GraphCacheAdapterP.h"
#include "GraphBGP.h"
#include "GraphCarP.h"

struct Handle_Cache {
	const char* path;
	int handle;
	int ref_count;
	Handle_Cache* next;
};

GraphCacheAdapter s = NULL;

static void init(Core p);
static void fin(Core p);
static void fin_obj(Core p);
static int get_or_create_handle(GraphCacheAdapter self, const char* path);

static GraphCacheAdapter GraphCacheAdapter_new(void) {
	GraphCacheAdapter g = (GraphCacheAdapter)new_instance(graphCacheAdapterClass);
	return g;
}

static GraphBase create_graph(GraphCacheAdapter self, GraphBaseClassDescriptor* clazz, const char* path, ...) {
	GraphBase ans = NULL;
	int handle = get_or_create_handle(self, path);

	va_list ap;
	va_start(ap, path);
	ans = clazz->base.constructor(handle, &ap);
	va_end(ap);

	if (ans == NULL) {
		return NULL;
	}
	ans->core.class_descriptor->core.finalizer = fin_obj;
	return ans;
}

GraphCacheAdapterClassDescriptor graph_cache_adapter_class_descriptor = {
	/* Core part */
	{
		"GraphCacheAdapter",             /* class_name */
		(CoreClassDescriptor*)&graph_base_class_descriptor,  /* super_class */
		sizeof(GraphCacheAdapterObj),    /* size_of_instance */
		NULL,                            /* class_initializer */
		init,                            /* initializer */
	    fin,                             /* finalizer */
	},
	/* GraphBase part */
	{
		NULL,                                /* constructor */
	},
	/* GraphCacheAdapter part */
	{
		GraphCacheAdapter_new,               /* constructor */
	},
};

CoreClassDescriptor* graphCacheAdapterClass = (CoreClassDescriptor*)&graph_cache_adapter_class_descriptor;

static void init(Core p) {
	GraphCacheAdapter g = (GraphCacheAdapter)p;
	Handle_Cache* cache = (Handle_Cache*)malloc(sizeof(Handle_Cache));
	g->gcache.cache = cache;
	cache->path = NULL;
	cache->handle = -1;
	cache->ref_count = 0;
	cache->next = NULL;

	g->gcache.create_graph = create_graph;

	s = g;
}

static void fin(Core p) {
	GraphCacheAdapter g = (GraphCacheAdapter)p;
	Handle_Cache* cache = g->gcache.cache;
	while (cache != NULL) {
		DeleteGraph(cache->handle);
		Handle_Cache* next = cache->next;
		free(cache);
		cache = next;
	}
	free(g);
	p = NULL;
	s = NULL;
}

static int get_or_create_handle(GraphCacheAdapter self, const char* path) {
	Handle_Cache* cache = self->gcache.cache;
	Handle_Cache* prev = NULL;
	while (cache != NULL) {
		if (cache->handle == -1) {
			cache->path = path;
			cache->handle = LoadGraph(path);
			cache->ref_count = 1;
			return cache->handle;
		}
		if (strcmp(cache->path, path) == 0) {
			cache->ref_count++;
			return cache->handle;
		}
		prev = cache;
		cache = cache->next;
	}
	cache = (Handle_Cache*)malloc(sizeof(Handle_Cache));
	cache->path = path;
	cache->handle = LoadGraph(path);
	cache->ref_count = 1;
	cache->next = NULL;
	if (prev != NULL) {
		prev->next = cache;
	}
	return cache->handle;
}

/*
*  return 0: success, -1: failed
*/
static int release_handle(GraphCacheAdapter self, int handle) {
	Handle_Cache* cache = self->gcache.cache;
	Handle_Cache* prev = NULL;
	while (cache != NULL) {
		if (cache->handle == handle) {
			cache->ref_count--;
			if (cache->ref_count == 0) {
				DeleteGraph(cache->handle);
				if (prev != NULL) {
					prev->next = cache->next;
				}
				free(cache);
			}
			return 0;
		}
		prev = cache;
		cache = cache->next;
	}
	return -1;
}

static void fin_obj(Core p) {
	GraphBase obj = (GraphBase)p;
	release_handle(s, obj->base.image);
	free(obj);
	p = NULL;
}

