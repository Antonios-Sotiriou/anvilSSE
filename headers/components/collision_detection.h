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
extern float movScalar;
extern vec4f gravity_epicenter;
extern Scene scene;

float sweptEdgeVsEdge(face *sf, face *mf, vec4f velocity, vec4f *n);
vec4f adjust_precission(vec4f v, int prec_lvl);
const float dist(vec4f plane_p, vec4f plane_n, vec4f v);
const vec4f plane_intersect(vec4f plane_p, vec4f plane_n, vec4f line_start, vec4f line_end, float *t);
const int terrainCollision(Mesh *terrain, Mesh *obj);
const void terrainHeightDifference(Mesh *terrain, Mesh *obj);
const int aabbCollision(TerrainInfo *ti, Scene *s, Mesh *obj);
const int velocityCollideTest(Mesh *m);
const int obbCollision(Mesh *m);
const int rotationCollision(TerrainInfo *ti, Scene *s, Mesh *obj);
const void sortCollisions(TerrainInfo *ti, Scene *s, Mesh *obj);

#endif /* _COLLISION_DETECTION_H */



