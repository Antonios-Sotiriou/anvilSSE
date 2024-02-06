#ifndef _TERRAIN_FUNCTIONS_H
#define _TERRAIN_FUNCTIONS_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _GENERAL_FUNCTIONS_H
    #include "general_functions.h"
#endif

#ifndef _MATRICES_H
    #include "matrices.h"
#endif

#ifndef _CLIPPING_H
    #include "clipping.h"
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

/* Defined in main.c. */
extern TerrainInfo tf;

const void initTerrainInfo(TerrainInfo *t);
const float getTerrainHeight(Mesh *t, vec4f coords, Mesh *m);
const int getTerrainQuadIndex(Mesh *t, vec4f coords);
const void addMeshToQuad(Mesh *m);
const void printQuad(const int quad_index);

#endif /* _TERRAIN_FUNCTIONS_H */



