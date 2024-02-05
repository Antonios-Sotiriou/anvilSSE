#ifndef _TEST_SHAPE_H
#define _TEST_SHAPE_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

#ifndef _STRING_H
    #include <string.h>
#endif

/* Defined in main.c. */
extern TerrainInfo tf;

const void createCube(Mesh *c);
const void createskybox(Mesh *c);
const void createPlane(Mesh *c);
const void createGrid(Mesh *c, int vrows, int vcols);
const void createTerrain(Mesh *c, const char path[]);

#endif /* _TEST_SHAPE_H */