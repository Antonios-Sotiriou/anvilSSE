#include "headers/scene_objects.h"

#define MESH_NUM 2

/* This function is responsible to position the objects in world space. */
const void initWorldObjects(Scene *s) {
    /* ######################################################################################################## */
    // memcpy(s->m[0].name, "basic_terrain", sizeof("basic_terrain"));
    // createGrid(&s->m[0], 10, 10);
    createTerrain(&s->m[0], "textures/height_map_bn1.bmp");
    loadmaterial(&s->m[0].material, "pearl");

    s->m[0].type = Terrain;
    s->m[0].id = 0;

    s->m[0].scale = 1000.f;

    s->m[0].pivot[0] = 500.f;
    s->m[0].pivot[1] = 0.f;
    s->m[0].pivot[2] = 500.f;

    s->m[0].cull = 1;
    s->m[0].lodlevels = 0;
    s->m[0].material.reflect = 0;
    s->m[0].Q = unitQuat();

    /* ######################################################################################################## */
    memcpy(s->m[1].name, "planet", sizeof("planet"));
    loadmaterial(&s->m[1].material, "earth");

    s->m[1].type = Player;
    s->m[1].id = 1;

    s->m[1].scale = 10.f;

    s->m[1].pivot[0] = 0.f;
    s->m[1].pivot[1] = 0.f;
    s->m[1].pivot[2] = 0.f;

    s->m[1].cull = 1;
    s->m[1].lodlevels = 5;
    // s->m[1].visible = 1;
    // s->m[1].material.reflect = 1;
    s->m[1].Q = unitQuat();

    /* ######################################################################################################## */
    // for (int i = 2; i < MESH_NUM; i++) {
    //     createCube(&s->m[i]);
    //     loadmaterial(&s->m[i].material, "jade");

    //     s->m[i].type = MovingObject;
    //     s->m[i].id = i;
        
    //     s->m[i].scale = 1.f;
    //     s->m[i].pivot[0] = (float)rand() / (float)(RAND_MAX / 400);
    //     s->m[i].pivot[1] = (float)rand() / (float)(RAND_MAX / 100);
    //     s->m[i].pivot[2] = (float)rand() / (float)(RAND_MAX / 400);
    //     s->m[i].cull = 1;
    //     s->m[i].lodlevels = 0;
    //     // s->m[1].visible = 1;
    //     s->m[i].Q = unitQuat();
    // }
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


