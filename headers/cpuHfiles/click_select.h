#ifndef _CLICK_SELECT_H
#define _CLICK_SELECT_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _MATRICES_H
    #include "matrices.h"
#endif

#ifndef _TERRAIN_FUNCTIONS_H
    #include "terrain_functions.h"
#endif

/* Defined in main.c. */
extern TerrainInfo Gitana;
extern Scene scene;
extern XWindowAttributes *point_attrib;
extern Mat4x4 reperspMatrix, *point_mat;

const int clickSelect(const vec4f cs);
const int pointVsBbox(TerrainInfo *tf, Scene *s, vec4f wsc);

#endif /* _CLICK_SELECT_H */


