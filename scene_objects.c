#include "headers/scene_objects.h"

#define MESH_NUM 1000

/* This function is responsible to position the objects in world space. */
const void initWorldObjects(Scene *s) {
    /* ######################################################################################################## */
    // memcpy(s->m[0].name, "basic_terrain", sizeof("basic_terrain"));
    // createPlane(&s->m[0]);
    // int x_inc = 200, z_inc = 200;
    // int scale = 100;
    // for (int i = 0; i < 25; i++) {
    //     createTerrain(&s->m[i], 10, 10);
    //     s->m[i].type = Terrain;
    //     loadmaterial(&s->m[i].material, "pearl");

    //     s->m[i].scale = scale;

    //     s->m[i].pivot[0] = x_inc + scale;
    //     s->m[i].pivot[1] = 0.f;
    //     s->m[i].pivot[2] = z_inc + scale;

    //     s->m[i].cull = 1;
    //     s->m[i].lodlevels = 0;
    //     // s->m[0].visible = 1;
    //     // s->m[0].material.reflect = 1;
    //     s->m[i].Q = unitQuat();
    //     reWorldMesh(&s->m[i]);

    // }
    /* ######################################################################################################## */
    // memcpy(s->m[0].name, "basic_terrain", sizeof("basic_terrain"));
    createTerrain(&s->m[0], 10, 10);
    s->m[0].type = Terrain;
    loadmaterial(&s->m[0].material, "pearl");

    s->m[0].scale = 100.f;

    s->m[0].pivot[0] = 100.f;
    s->m[0].pivot[1] = 0.f;
    s->m[0].pivot[2] = 100.f;

    s->m[0].cull = 1;
    s->m[0].lodlevels = 0;
    s->m[0].Q = unitQuat();
    reWorldMesh(&s->m[0]);

    /* ######################################################################################################## */
    // memcpy(s->m[1].name, "basic_terrain", sizeof("basic_terrain"));
    createTerrain(&s->m[1], 10, 10);
    loadmaterial(&s->m[1].material, "pearl");

    s->m[1].type = Terrain;

    s->m[1].scale = 100.f;

    s->m[1].pivot[0] = 100.f;
    s->m[1].pivot[1] = 0.f;
    s->m[1].pivot[2] = 300.f;
    s->m[1].cull = 1;
    s->m[1].lodlevels = 0;
    s->m[1].Q = unitQuat();
    reWorldMesh(&s->m[1]);

    // /* ######################################################################################################## */
    // memcpy(s->m[2].name, "basic_terrain", sizeof("basic_terrain"));
    createTerrain(&s->m[2], 10, 10);
    loadmaterial(&s->m[2].material, "pearl");

    s->m[2].type = Terrain;

    s->m[2].scale = 100.f;

    s->m[2].pivot[0] = 300.f;
    s->m[2].pivot[1] = 0.f;
    s->m[2].pivot[2] = 100.f;
    s->m[2].cull = 1;
    s->m[2].lodlevels = 0;
    s->m[2].Q = unitQuat();
    reWorldMesh(&s->m[2]);

    // /* ######################################################################################################## */
    // memcpy(s->m[3].name, "basic_terrain", sizeof("basic_terrain"));
    createTerrain(&s->m[3], 10, 10);
    loadmaterial(&s->m[3].material, "pearl");

    s->m[3].type = Terrain;

    s->m[3].scale = 100.f;

    s->m[3].pivot[0] = 300.f;
    s->m[3].pivot[1] = 0.f;
    s->m[3].pivot[2] = 300.f;
    s->m[3].cull = 1;
    s->m[3].lodlevels = 0;
    s->m[3].Q = unitQuat();
    reWorldMesh(&s->m[3]);


    /* ######################################################################################################## */
    memcpy(s->m[4].name, "planet", sizeof("planet"));
    loadmaterial(&s->m[4].material, "emerald");

    s->m[4].type = Player;

    s->m[4].scale = 10.f;

    s->m[4].pivot[0] = 0.f;
    s->m[4].pivot[1] = 1000.f;
    s->m[4].pivot[2] = 100.f;

    s->m[4].cull = 1;
    s->m[4].lodlevels = 5;
    // s->m[1].visible = 1;
    // s->m[1].material.reflect = 1;
    s->m[4].Q = unitQuat();

    /* ######################################################################################################## */
    for (int i = 5; i < MESH_NUM; i++) {
        // memcpy(s->m[i].name, "planet", sizeof("planet"));
        createCube(&s->m[i]);
        loadmaterial(&s->m[i].material, "jade");

        s->m[i].scale = 1.f;
        s->m[i].pivot[0] = (float)rand() / (float)(RAND_MAX / 400);
        s->m[i].pivot[1] = (float)rand() / (float)(RAND_MAX / 100);
        s->m[i].pivot[2] = (float)rand() / (float)(RAND_MAX / 400);
        s->m[i].cull = 1;
        s->m[i].lodlevels = 0;
        // s->m[1].visible = 1;
        s->m[i].type = MovingObject;
        s->m[i].Q = unitQuat();
        reWorldMesh(&s->m[i]);
    }

    for (int i = 0; i < s->m_indexes; i++) {
        adoptdetailMesh(&s->m[i]);
        adoptdetailTexture(&s->m[i]);
    }

    // printf("Struct size Mesh: %lu\n", sizeof(Mesh) - (sizeof(Material) - sizeof(Quat)));
    // printf("Struct size Material: %d\n", sizeof(Material));
    // printf("Struct size Quat: %d\n", sizeof(Quat));
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


