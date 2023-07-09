#ifndef _WORLD_OBJECTS_H
#define _WORLD_OBJECTS_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _MATRICES_H
    #include "matrices.h"
#endif

#ifndef _TEST_SHAPES_H
    #include "test_shapes.h"
#endif

const void posWorldObjects(Scene *s);
const void createScene(Scene *s);
const void releaseScene(Scene *s);
const void releaseMesh(Mesh *c);
const void initMesh(Mesh *a, const Mesh b);

#endif /* _WORLD_OBJECTS_H */