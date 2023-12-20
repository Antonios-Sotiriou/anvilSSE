#include "headers/world_objects.h"

/* This function is responsible to position the objects in world space. */
const void initWorldObjects(Scene *s) {

    // Mesh ms[3] = {
    //     {
    //         .name = "cubeflat",
    //         .scale = 100.f,
    //         .pivot = { 100.f, 0.f, 0.f },
    //         .rotation = { 0.f, 90.f, 0.f },
    //         .cull = 1,
    //         .lodlevels = 1,
    //     } 
    // };
    // s->m[0] = ms[0];
    // loadmaterial(&s->m[0].material, "earth");

    /* ######################################################################################################## */
    memcpy(s->m[0].name, "planet", sizeof("planet"));
    loadmaterial(&s->m[0].material, "earth");

    s->m[0].scale = 100.f;
    s->m[0].pivot[0] = 100.f;
    s->m[0].rotation[1] = 90.f;
    s->m[0].cull = 1;
    s->m[0].lodlevels = 6;

    /* ######################################################################################################## */
    memcpy(s->m[1].name, "planet", sizeof("planet"));
    loadmaterial(&s->m[1].material, "gold");

    s->m[1].scale = 100.f;
    s->m[1].pivot[0] = -100.f;
    s->m[1].cull = 1;
    s->m[1].lodlevels = 6;

    /* ######################################################################################################## */
    memcpy(s->m[2].name, "planet", sizeof("planet"));
    loadmaterial(&s->m[2].material, "light");

    s->m[2].scale = 10.f;
    s->m[2].pivot[1] = 100.f;
    s->m[2].cull = 1;
    s->m[2].lodlevels = 6;
}
/* Teams all objects of the the world in a scene for further procesing. */
const void createScene(Scene *s) {
    s->m = calloc(3, sizeof(Mesh));
    s->m_indexes = 3;
}
/* Releases all Meshes that form a scene. */
const void releaseScene(Scene *s) {
    for (int i = 0; i < s->m_indexes; i++) {
        free(s->m[i].material.texture);
        releaseMesh(&s->m[i]);
    }
    free(s->m);
}


