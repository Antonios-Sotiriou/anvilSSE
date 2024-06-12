#ifndef _BOUNDINGBOX_LOADER_H
#define _BOUNDINGBOX_LOADER_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

// #ifndef _STDLIB_H
//     #include <stdlib.h>
// #endif

#ifndef _STRING_H
    #include <string.h>
#endif

const void loadBoundingBox(Bbox *bb, const char name[]);

#endif /* _BOUNDINGBOX_LOADER_H */


