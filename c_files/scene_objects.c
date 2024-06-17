#include "../headers/scene_objects.h"

#define MESH_NUM 3

/* This function is responsible to position the objects in world space. */
const void initWorldObjects(Scene *s) {
    /* ######################################################################################################## */
    // createGrid(&s->m[0], 10, 10);

    memcpy(s->m[0].name, "terrain0", sizeof("terrain0"));
    createTerrain(&s->m[0], "terrains/terrain0/hm_10x10.bmp");
    loadMaterial(&s->m[0].material, "obsidian");
    loadBoundingBox(&s->m[0].bbox, "terrains/terrain0/bbox_terrain0");

    s->m[0].type = Terrain;
    s->m[0].id = 0;

    s->m[0].scale = 1000.f;

    s->m[0].pivot[0] = 1000.f;
    s->m[0].pivot[1] = 0.f;
    s->m[0].pivot[2] = 1000.f;

    s->m[0].cull = 1;
    s->m[0].lodlevels = 0;
    // s->m[0].material.reflect = 1;
    s->m[0].mass = 0;
    s->m[0].Q = unitQuat();
    enWorldMesh(&s->m[0]);     /* Terrain must be in world Space to check for collisions. */
    enWorldBoundingBox(&s->m[0]);

    /* ######################################################################################################## */
    memcpy(s->m[1].name, "planet", sizeof("planet"));
    // createCube(&s->m[1]);
    loadMaterial(&s->m[1].material, "stones");
    loadMesh(&s->m[1], "objfiles/planet", 1);
    loadBoundingBox(&s->m[1].bbox, "objfiles/cubesmooth/bbox_cubesmooth");

    s->m[1].type = Player;
    s->m[1].id = 1;

    s->m[1].scale = 10.f;

    s->m[1].pivot[0] = 0.f;
    s->m[1].pivot[1] = 0.f;
    s->m[1].pivot[2] = 0.f;

    s->m[1].cull = 1;
    s->m[1].lodlevels = 5;
    // s->m[1].visible = 1;
    s->m[1].mass = 0.5;
    // s->m[1].grounded = 1;
    // s->m[1].material.reflect = 1;
    s->m[1].Q = unitQuat();
    // enWorldMesh(&s->m[1]);
    enWorldBoundingBox(&s->m[1]);

    // /* ######################################################################################################## */
    createCube(&s->m[2]);
    memcpy(s->m[2].name, "cubesmooth", sizeof("cubesmooth"));
    loadMesh(&s->m[2], "objfiles/cubesmooth", 1);
    loadMaterial(&s->m[2].material, "jade");
    loadBoundingBox(&s->m[2].bbox, "objfiles/cubesmooth/bbox_cubesmooth");

    s->m[2].type = MovingObject;
    s->m[2].id = 2;
    
    s->m[2].scale = 10.f;

    s->m[2].pivot[0] = 100;
    s->m[2].pivot[1] = 0;
    s->m[2].pivot[2] = 100;
    s->m[2].cull = 1;
    s->m[2].lodlevels = 0;
    // s->m[2].visible = 1;
    s->m[2].mass = 0.5;
    // s->m[2].grounded = 1;
    s->m[2].Q = unitQuat();
    // enWorldMesh(&s->m[2]);
    enWorldBoundingBox(&s->m[2]);

    // /* ######################################################################################################## */
    // createCube(&s->m[4]);
    // loadMaterial(&s->m[4].material, "jade");
    // loadBoundingBox(&s->m[4].bbox, "objfiles/cubesmooth/bbox_cubesmooth");

    // s->m[4].type = MovingObject;
    // s->m[4].id = 4;
    
    // s->m[4].scale = 10.f;
    // s->m[4].pivot[0] = 80;
    // s->m[4].pivot[1] = 30;
    // s->m[4].pivot[2] = 130;
    // s->m[4].cull = 1;
    // s->m[4].lodlevels = 0;
    // // s->m[4].visible = 1;
    // s->m[4].mass = 0.5;
    // // s->m[4].grounded = 1;
    // s->m[4].Q = unitQuat();
    // // enWorldMesh(&s->m[4]);
    // enWorldBoundingBox(&s->m[4]);

    // /* ######################################################################################################## */
    // createCube(&s->m[5]);
    // loadMaterial(&s->m[5].material, "jade");
    // loadBoundingBox(&s->m[5].bbox, "objfiles/cubesmooth/bbox_cubesmooth");

    // s->m[5].type = MovingObject;
    // s->m[5].id = 5;
    
    // s->m[5].scale = 10.f;
    // s->m[5].pivot[0] = 80;
    // s->m[5].pivot[1] = 0;
    // s->m[5].pivot[2] = 100;
    // s->m[5].cull = 1;
    // s->m[5].lodlevels = 0;
    // // s->m[5].visible = 1;
    // s->m[5].mass = 0.5;
    // // s->m[5].grounded = 1;
    // s->m[5].Q = unitQuat();
    // // enWorldMesh(&s->m[5]);
    // enWorldBoundingBox(&s->m[5]);

    /* ######################################################################################################## */
    // memcpy(s->m[2].name, "spacedomsmooth", sizeof("spacedomsmooth"));
    // loadMaterial(&s->m[2].material, "spacedom");
    // loadMesh(&s->m[2], "objfiles/spacedomsmooth", 1);
    // loadBoundingBox(&s->m[2].bbox, "objfiles/planet/bbox_planet");

    // s->m[2].type = Celestial;
    // s->m[2].id = 2;

    // s->m[2].scale = 10000.f;

    // s->m[2].pivot[0] = 0.f;
    // s->m[2].pivot[1] = 0.f;
    // s->m[2].pivot[2] = 0.f;

    // s->m[2].cull = 1;
    // s->m[2].lodlevels = 1;
    // // s->m[2].visible = 1;
    // s->m[2].mass = 0;
    // // s->m[2].material.reflect = 1;
    // s->m[2].Q = unitQuat();
    // // enWorldMesh(&s->m[2]);
    // enWorldBoundingBox(&s->m[2]);

    /* ######################################################################################################## */
    // for (int i = 5; i < MESH_NUM; i++) {
    //     createCube(&s->m[i]);
    //     loadMaterial(&s->m[i].material, "jade");

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
        // enWorldMesh(&s->m[i]);
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
        free(s->m[i].bbox.v);
    }
    free(s->m);
}


