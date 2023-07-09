#include "headers/world_objects.h"

/* Enumerate objects to form the scene. Add every new objects name at 1 position before the end, AKA ( before LastObj entry). */
enum { Cube, LastObj };

/* This function is responsible to position the objects in world space. */
const void posWorldObjects(Scene *s) {
    Mesh cube = { 0 };
    createCube(&cube);
    Mat4x4 sclMatrix, trMatrix, posMatrix;
    
    sclMatrix = scaleMatrix(1.0);
    trMatrix = translationMatrix(0.0, 0.0, 0.0);
    posMatrix = mxm(sclMatrix, trMatrix);

    initMesh(&s->m[0], cube);
    s->m[Cube].v = meshxm(cube.v, cube.v_indexes, posMatrix);

    releaseMesh(&cube);
}
/* Teams all objects of the the world in a scene for further procesing. */
const void createScene(Scene *s) {
    s->m = malloc(sizeof(Mesh) * LastObj);
    s->m_indexes = LastObj;
}
/* Releases all Meshes that form a scene. */
const void releaseScene(Scene *s) {
    for (int i = 0; i < s->m_indexes; i++) {
        releaseMesh(&s->m[i]);
    }
}
/* Releases all members of the given Mesh. */
const void releaseMesh(Mesh *c) {
    free(c->v);
    free(c->f);
}
/* Initializing Mesh a from Mesh b. */
const void initMesh(Mesh *a, const Mesh b) {
    *a = b;

    a->f = malloc(36 * b.f_indexes);
    memcpy(a->f, b.f, 36 * b.f_indexes);
}


