#ifndef _COLLISION_DETECTION_H
#define _COLLISION_DETECTION_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _MATRICES_H
    #include "matrices.h"
#endif

#ifndef _TERRAIN_FUNCTIONS_H
    #include "terrain_functions.h"
#endif

const void objectTerrainCollision(Mesh *terrain, Mesh *object);
const int objectEnvironmentCollision(TerrainInfo *tf, Scene *s, Mesh *obj, vec4f *velocity);

#endif /* _COLLISION_DETECTION_H */



