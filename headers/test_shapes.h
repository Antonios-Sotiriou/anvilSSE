#ifndef _TEST_SHAPE_H
#define _TEST_SHAPE_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _STRING_H
    #include "string.h"
#endif

const void createCube(Mesh *c);
const void createskybox(Mesh *c);
const void createPlane(Mesh *c);
const void createTerrain(Mesh *c, const int vx, const int vz);

#endif /* _TEST_SHAPE_H */