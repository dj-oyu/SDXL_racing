#include <stdio.h>
#include "CoreP.h"

CoreClassDescriptor core_class_descriptor = {
    /* Core part */
    {
        "Core",                 /* class_name */
        NULL,                   /* super_class */
        sizeof(CoreObj),        /* size_of_instance */
        NULL,                   /* class_initializer */
        NULL,                   /* initializer */
        NULL,                   /* finalizer */
    },
};

CoreClassDescriptor* coreClass = &core_class_descriptor;