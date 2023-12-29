#include "headers/scene_objects.h"

#define MESH_NUM 5

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
    memcpy(s->m[0].name, "basic_terrain", sizeof("basic_terrain"));
    loadmaterial(&s->m[0].material, "stones");

    s->m[0].scale = 100.f;
    s->m[0].pivot[0] = 100.f;
    s->m[0].rotation[1] = 90.f;
    s->m[0].cull = 1;
    s->m[0].lodlevels = 5;

    /* ######################################################################################################## */
    memcpy(s->m[1].name, "basic_terrain", sizeof("basic_terrain"));
    loadmaterial(&s->m[1].material, "stones");

    s->m[1].scale = 100.f;
    s->m[1].pivot[0] = 300.f;
    s->m[1].cull = 1;
    s->m[1].lodlevels = 5;

    /* ######################################################################################################## */
    memcpy(s->m[2].name, "basic_terrain", sizeof("basic_terrain"));
    loadmaterial(&s->m[2].material, "stones");

    s->m[2].scale = 100.f;
    s->m[2].pivot[0] = 100.f;
    s->m[2].pivot[2] = 200.f;
    s->m[2].cull = 1;
    s->m[2].lodlevels = 5;

    /* ######################################################################################################## */
    memcpy(s->m[3].name, "basic_terrain", sizeof("basic_terrain"));
    loadmaterial(&s->m[3].material, "stones");

    s->m[3].scale = 100.f;
    s->m[3].pivot[0] = 300.f;
    s->m[3].pivot[2] = 200.f;
    s->m[3].cull = 1;
    s->m[3].lodlevels = 5;

    /* ######################################################################################################## */
    memcpy(s->m[4].name, "planet", sizeof("planet"));
    loadmaterial(&s->m[4].material, "light");

    s->m[4].scale = 10.f;
    s->m[4].pivot[0] = 300.f;
    s->m[4].pivot[2] = 200.f;
    s->m[4].cull = 1;
    s->m[4].lodlevels = 6;

    for (int i = 0; i < s->m_indexes; i++) {
        adoptdetailMesh(&s->m[i]);
        adoptdetailTexture(&s->m[i]);
    }
}
/* Teams all objects of the the world in a scene for further procesing. */
const void createScene(Scene *s) {
    s->m = calloc(MESH_NUM, sizeof(Mesh));
    s->m_indexes = MESH_NUM;
}
/* Releases all Meshes that form a scene. */
const void releaseScene(Scene *s) {
    for (int i = 0; i < s->m_indexes; i++) {
        free(s->m[i].material.texture);
        releaseMesh(&s->m[i]);
    }
    free(s->m);
}


