#ifndef _COLLISION_DETECTION_H
#define _COLLISION_DETECTION_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

const int checkTerrainCollision(Mesh *m, const float terrain_height);
const int getPossibleColliders(Scene *s);
const void collideRangeCulling(Mesh *m, const int len, Mesh *cl);

#endif /* _COLLISION_DETECTION_H */



