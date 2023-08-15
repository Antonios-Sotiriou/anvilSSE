#include "headers/world_objects.h"

extern float AmbientStrength, SpecularStrength;

/* This function is responsible to position the objects in world space. */
const void posWorldObjects(Scene *s) {
    Mesh cube = loadmesh("objfiles/terrain.obj");
    // createCube(&cube);
    Mat4x4 sclMatrix, trMatrix, posMatrix;
    
    sclMatrix = scaleMatrix(100.0);
    trMatrix = translationMatrix(0.0, 0.0, 0.0);
    posMatrix = mxm(sclMatrix, trMatrix);

    Material mat = {
        .basecolor = { 1.0f, 0.8f, 0.0f, 0.0f },
        .ambient = { 0.24725f, 0.1995f, 0.0745f, 0.0f },
        .specular = { 0.628281f, 0.555802f, 0.366065f, 0.0f },
        .diffuse = { 0.75164f, 0.60648f, 0.22648f, 0.0f },
        .shinniness = 0.4 * 128,
        .reflect = 0
    };
    cube.material = mat;

    initMesh(&s->m[0], cube);
    s->m[0].v = meshxm(cube.v, cube.v_indexes, posMatrix);
    s->m[0].n = meshxm(cube.n, cube.n_indexes, posMatrix);

    releaseMesh(&cube);
    /* ######################################################################################################## */
    Mesh jupiter = loadmesh("objfiles/earth.obj");
    sclMatrix = scaleMatrix(10.0);
    trMatrix = translationMatrix(-10.0, 0.0, 80.0);
    posMatrix = mxm(sclMatrix, trMatrix);

    jupiter.material = mat;
    jupiter.material.reflect = 1;

    initMesh(&s->m[1], jupiter);
    s->m[1].v = meshxm(jupiter.v, jupiter.v_indexes, posMatrix);
    s->m[1].n = meshxm(jupiter.n, jupiter.n_indexes, posMatrix);

    releaseMesh(&jupiter);
}
/* Teams all objects of the the world in a scene for further procesing. */
const void createScene(Scene *s) {
    s->m = malloc(sizeof(Mesh) * 2);
    s->m_indexes = 2;
}
/* Releases all Meshes that form a scene. */
const void releaseScene(Scene *s) {
    for (int i = 0; i < s->m_indexes; i++) {
        releaseMesh(&s->m[i]);
    }
    free(s->m);
}
/* Releases all members of the given Mesh. */
const void releaseMesh(Mesh *c) {
    free(c->v);
    free(c->n);
    free(c->t);
    free(c->f);
}
/* Initializing Mesh a from Mesh b. */
const void initMesh(Mesh *a, const Mesh b) {
    *a = b;
    // size_t nsize = sizeof(vec4f) * b.n_indexes;
    size_t tsize = sizeof(vec4f) * b.t_indexes;
    size_t fsize = sizeof(face) * b.f_indexes;

    // a->n = malloc(nsize);
    // memcpy(a->n, b.n, nsize);

    a->t = malloc(tsize);
    memcpy(a->t, b.t, tsize);

    a->f = malloc(fsize);
    memcpy(a->f, b.f, fsize);
}


