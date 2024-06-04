#include "headers/scene_objects.h"

#define MESH_NUM 6

/* This function is responsible to position the objects in world space. */
const void initWorldObjects(Scene *s) {
    /* ######################################################################################################## */
    // memcpy(s->m[0].name, "basic_terrain", sizeof("basic_terrain"));
    // createGrid(&s->m[0], 10, 10);
    createTerrain(&s->m[0], "textures/height_map_10x10.bmp");
    loadmaterial(&s->m[0].material, "obsidian");

    s->m[0].type = Terrain;
    s->m[0].id = 0;

    s->m[0].scale = 10000.f;

    s->m[0].pivot[0] = 5000.f;
    s->m[0].pivot[1] = 0.f;
    s->m[0].pivot[2] = 5000.f;

    s->m[0].cull = 1;
    s->m[0].lodlevels = 0;
    // s->m[0].material.reflect = 1;
    s->m[0].mass = 0;
    s->m[0].Q = unitQuat();
    enWorldMesh(&s->m[0]);

    /* ######################################################################################################## */
    memcpy(s->m[1].name, "planet", sizeof("planet"));
    // createCube(&s->m[1]);
    loadmaterial(&s->m[1].material, "stones");
    loadmesh(&s->m[1], s->m[1].name, 1);

    s->m[1].type = Player;
    s->m[1].id = 1;

    s->m[1].scale = 10.f;

    s->m[1].pivot[0] = 90.f;
    s->m[1].pivot[1] = 0.f;
    s->m[1].pivot[2] = 75.f;

    s->m[1].cull = 1;
    s->m[1].lodlevels = 5;
    // s->m[1].visible = 1;
    s->m[1].mass = 0.5;
    // s->m[1].grounded = 1;
    // s->m[1].material.reflect = 1;
    s->m[1].Q = unitQuat();
    // enWorldMesh(&s->m[1]);

    /* ######################################################################################################## */
    memcpy(s->m[2].name, "spacedomsmooth", sizeof("spacedomsmooth"));
    // createCube(&s->m[3]);
    loadmaterial(&s->m[2].material, "spacedom");
    loadmesh(&s->m[2], s->m[2].name, 1);

    s->m[2].type = Terrain;
    s->m[2].id = 0;

    s->m[2].scale = 10000.f;

    s->m[2].pivot[0] = 0.f;
    s->m[2].pivot[1] = 0.f;
    s->m[2].pivot[2] = 0.f;

    s->m[2].cull = 1;
    s->m[2].lodlevels = 1;
    // s->m[2].visible = 1;
    s->m[2].mass = 0;
    // s->m[2].material.reflect = 1;
    s->m[2].Q = unitQuat();
    // enWorldMesh(&s->m[2]);

    /* ######################################################################################################## */
    createCube(&s->m[3]);
    loadmaterial(&s->m[3].material, "jade");

    s->m[3].type = MovingObject;
    s->m[3].id = 3;
    
    s->m[3].scale = 10.f;
    s->m[3].pivot[0] = 110;
    s->m[3].pivot[1] = 30;
    s->m[3].pivot[2] = 100;
    s->m[3].cull = 1;
    s->m[3].lodlevels = 0;
    // s->m[3].visible = 1;
    s->m[3].mass = 0.5;
    // s->m[3].grounded = 1;
    s->m[3].Q = unitQuat();
    enWorldMesh(&s->m[3]);

    // createCube(&s->m[3]);
    // loadmaterial(&s->m[3].material, "jade");

    // s->m[3].type = MovingObject;
    // s->m[3].id = 3;
    
    // s->m[3].scale = 100.f;
    // s->m[3].pivot[0] = 210;
    // s->m[3].pivot[1] = 30;
    // s->m[3].pivot[2] = 200;
    // s->m[3].cull = 1;
    // s->m[3].lodlevels = 0;
    // // s->m[3].visible = 1;
    // s->m[3].mass = 0.5;
    // // s->m[3].grounded = 1;
    // s->m[3].Q = unitQuat();
    // enWorldMesh(&s->m[3]);

    /* ######################################################################################################## */
    createCube(&s->m[4]);
    loadmaterial(&s->m[4].material, "jade");

    s->m[4].type = MovingObject;
    s->m[4].id = 4;
    
    s->m[4].scale = 10.f;
    s->m[4].pivot[0] = 80;
    s->m[4].pivot[1] = 30;
    s->m[4].pivot[2] = 130;
    s->m[4].cull = 1;
    s->m[4].lodlevels = 0;
    // s->m[4].visible = 1;
    s->m[4].mass = 0.5;
    // s->m[4].grounded = 1;
    s->m[4].Q = unitQuat();
    enWorldMesh(&s->m[4]);

    /* ######################################################################################################## */
    createCube(&s->m[5]);
    loadmaterial(&s->m[5].material, "jade");

    s->m[5].type = MovingObject;
    s->m[5].id = 5;
    
    s->m[5].scale = 10.f;
    s->m[5].pivot[0] = 80;
    s->m[5].pivot[1] = 0;
    s->m[5].pivot[2] = 100;
    s->m[5].cull = 1;
    s->m[5].lodlevels = 0;
    // s->m[5].visible = 1;
    s->m[5].mass = 0.5;
    // s->m[5].grounded = 1;
    s->m[5].Q = unitQuat();
    enWorldMesh(&s->m[5]);

    /* ######################################################################################################## */
    // for (int i = 5; i < MESH_NUM; i++) {
    //     createCube(&s->m[i]);
    //     loadmaterial(&s->m[i].material, "jade");

    //     s->m[i].type = MovingObject;
    //     s->m[i].id = i;
        
    //     s->m[i].scale = 3.f;
    //     s->m[i].pivot[0] = (float)rand() / (float)(RAND_MAX / 400);
    //     s->m[i].pivot[1] = (float)rand() / (float)(RAND_MAX / 100);
    //     s->m[i].pivot[2] = (float)rand() / (float)(RAND_MAX / 400);
    //     s->m[i].cull = 1;
    //     s->m[i].lodlevels = 0;
    //     // s->m[1].visible = 1;
    //     s->m[i].mass = 0.5;
    //     // s->m[i].grounded = 1;
    //     s->m[i].Q = unitQuat();
    //     enWorldMesh(&s->m[i]);
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


