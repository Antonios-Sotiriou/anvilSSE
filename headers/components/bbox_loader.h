#ifndef _BBOX_LOADER_H
#define _BBOX_LOADER_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

#ifndef _STRING_H
    #include <string.h>
#endif

const void loadBboxVectors(Mesh *m, const char name[]);
const void loadBboxFaces(Mesh *m, const char path[]);

#endif /* _BBOX_LOADER_H */


