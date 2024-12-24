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

/* Defined in main.c */
extern int COLLISION, COLLIDINGFACE, COLLIDINGMESH;
extern float DeltaTime;
extern const float movScalar;
extern vec4f gravity_epicenter;
extern Scene scene;

const void terrainCollision(Mesh *terrain, Mesh *obj);
const void terrainHeightDifference(Mesh *terrain, Mesh *obj);
const int aabbCollision(TerrainInfo *ti, Scene *s, Mesh *obj);
const int obbCollision(Mesh *m);
const int rotationCollision(TerrainInfo *ti, Scene *s, Mesh *obj);
const void sortCollisions(TerrainInfo *ti, Scene *s, Mesh *obj);

#endif /* _COLLISION_DETECTION_H */



