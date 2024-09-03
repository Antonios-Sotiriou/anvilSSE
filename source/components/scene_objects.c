#include "../../headers/components/scene_objects.h"
#define MESH_NUM 8

/* This function is responsible to position the objects in world space. */
const void initWorldObjects(Scene *s) {
    /* ######################################################################################################## */
    // createGrid(&s->m[0], 10, 10);

    memcpy(s->m[0].name, "terrain0", sizeof("terrain0"));
    createTerrain(&s->m[0], "terrains/terrain0/hm_10x10.bmp", &Gitana);
    loadMaterial(&s->m[0].material, "stones");
    loadtexture(&s->m[0], 1);
    loadBboxVectors(&s->m[0], "terrains/terrain0/bbox_terrain0");
    loadBboxFaces(&s->m[0], "terrains/terrain0/lod2");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, testTexture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, s->m[0].material.texture_width, s->m[0].material.texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->m[0].material.texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    free(s->m[0].material.texture);

    if ( (texLoc0 = glGetUniformLocation(mainShaderProgram, "ourTexture[0]")) != 6 )
        fprintf(stderr, "Could not locate uniform variable with name: ourTexture[0]. Error: %d\n", texLoc0);
    glUniform1i(texLoc0, 0);

    s->m[0].type = Terrain;
    s->m[0].id = 0;
    s->m[0].tex_index = 0;

    s->m[0].scale = 50000.f;

    s->m[0].cd.v[P][0] = 50000.f;
    s->m[0].cd.v[P][1] = 0.f;
    s->m[0].cd.v[P][2] = 50000.f;
    s->m[0].cd.v[P][3] = 1.f;

    s->m[0].cull = 1;
    s->m[0].lodlevels = 0;
    // s->m[0].material.reflect = 1;
    s->m[0].mass = 0;
    s->m[0].Q = unitQuat();
    // enWorldMesh(&s->m[0]);     /* Terrain must be in world Space to check for collisions. */
    enworldBbox(&s->m[0]);

    /* ######################################################################################################## */
    memcpy(s->m[1].name, "cubesmooth", sizeof("cubesmooth"));
    // createCube(&s->m[1]);
    loadMaterial(&s->m[1].material, "earth");
    loadtexture(&s->m[1], 1);
    loadMesh(&s->m[1], "cubesmooth", 1);
    loadBboxVectors(&s->m[1], "objfiles/cubesmooth/bbox_cubesmooth");
    loadBboxFaces(&s->m[1], "objfiles/cubesmooth/lod1");

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, testTexture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, s->m[1].material.texture_width, s->m[1].material.texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->m[1].material.texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    free(s->m[1].material.texture);

    if ( (texLoc1 = glGetUniformLocation(mainShaderProgram, "ourTexture[1]")) != 7 )
        fprintf(stderr, "Could not locate uniform variable with name: ourTexture[1]. Error: %d\n", texLoc1);
    glUniform1i(texLoc1, 1);

    s->m[1].type = Player;
    s->m[1].id = 1;
    s->m[1].tex_index = 1;

    s->m[1].scale = 100.f;

    s->m[1].cd.v[P][0] = 800.f;
    s->m[1].cd.v[P][1] = 300.f;
    s->m[1].cd.v[P][2] = 100.f;
    s->m[1].cd.v[P][3] = 1.f;

    s->m[1].cull = 1;
    s->m[1].lodlevels = 1;
    // s->m[1].visible = 1;
    s->m[1].weight = 82 * 9.81;
    s->m[1].mass = s->m[1].weight / 9.81;
    s->m[1].fr_coef = 0.5;
    // s->m[1].grounded = 1;
    // s->m[1].material.reflect = 1;
    s->m[1].Q = unitQuat();
    // enWorldMesh(&s->m[1]);
    enworldBbox(&s->m[1]);

    // /* ######################################################################################################## */
    // createCube(&s->m[2]);
    memcpy(s->m[2].name, "cubesmooth", sizeof("cubesmooth"));
    loadMesh(&s->m[2], "cubesmooth", 1);
    loadMaterial(&s->m[2].material, "gold");
    loadtexture(&s->m[2], 1);
    loadBboxVectors(&s->m[2], "objfiles/cubesmooth/bbox_cubesmooth");
    loadBboxFaces(&s->m[2], "objfiles/cubesmooth/lod1");

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, testTexture[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, s->m[2].material.texture_width, s->m[2].material.texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->m[2].material.texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    free(s->m[2].material.texture);

    if ( (texLoc2 = glGetUniformLocation(mainShaderProgram, "ourTexture[2]")) != 8 )
        fprintf(stderr, "Could not locate uniform variable with name: ourTexture[2]. Error: %d\n", texLoc2);
    glUniform1i(texLoc2, 2);

    s->m[2].type = MovingObject;
    s->m[2].id = 2;
    s->m[2].tex_index = 2;
    
    s->m[2].scale = 500.f;

    s->m[2].cd.v[P][0] = 2000;
    s->m[2].cd.v[P][1] = 0;
    s->m[2].cd.v[P][2] = 2000;
    s->m[2].cd.v[P][3] = 1.f;

    s->m[2].cull = 1;
    s->m[2].lodlevels = 0;
    // s->m[2].visible = 1;
    s->m[2].mass = 0.5;
    s->m[2].fr_coef = 0.5;
    // s->m[2].grounded = 1;
    s->m[2].Q = rotationQuat(45, (vec4f){ 0.f, 1.f, 0.f });
    // enWorldMesh(&s->m[2]);
    enworldBbox(&s->m[2]);

    /* ######################################################################################################## */
    createCube(&s->m[3]);
    loadMaterial(&s->m[3].material, "jade");
    loadBboxVectors(&s->m[3], "objfiles/cubesmooth/bbox_cubesmooth");
    loadBboxFaces(&s->m[3], "objfiles/cubesmooth/lod1");

    s->m[3].type = MovingObject;
    s->m[3].id = 3;
    
    s->m[3].scale = 100.f;

    s->m[3].cd.v[P][0] = 10000;
    s->m[3].cd.v[P][1] = 300;
    s->m[3].cd.v[P][2] = 10200;
    s->m[3].cd.v[P][3] = 1.f;

    s->m[3].cull = 1;
    s->m[3].lodlevels = 0;
    // s->m[3].visible = 1;
    s->m[3].mass = 0.5;
    s->m[3].fr_coef = 0.5;
    // s->m[3].grounded = 1;
    s->m[3].Q = unitQuat();
    // enWorldMesh(&s->m[3]);
    enworldBbox(&s->m[3]);

    /* ######################################################################################################## */
    createCube(&s->m[4]);
    loadMaterial(&s->m[4].material, "jade");
    loadBboxVectors(&s->m[4], "objfiles/cubesmooth/bbox_cubesmooth");
    loadBboxFaces(&s->m[4], "objfiles/cubesmooth/lod1");

    s->m[4].type = MovingObject;
    s->m[4].id = 4;
    
    s->m[4].scale = 100.f;

    s->m[4].cd.v[P][0] = 10200;
    s->m[4].cd.v[P][1] = 300;
    s->m[4].cd.v[P][2] = 10000;
    s->m[4].cd.v[P][3] = 1.f;

    s->m[4].cull = 1;
    s->m[4].lodlevels = 0;
    // s->m[4].visible = 1;
    s->m[4].mass = 0.5;
    s->m[4].fr_coef = 0.5;
    // s->m[4].grounded = 1;
    s->m[4].Q = unitQuat();
    // enWorldMesh(&s->m[4]);
    enworldBbox(&s->m[4]);

    /* ######################################################################################################## */
    memcpy(s->m[5].name, "spacedomsmooth", sizeof("spacedomsmooth"));
    loadMaterial(&s->m[5].material, "spacedom");
    loadMesh(&s->m[5], "spacedomsmooth", 1);
    loadBboxVectors(&s->m[5], "objfiles/planet/bbox_planet");
    loadBboxFaces(&s->m[5], "objfiles/spacedomsmooth/lod1");

    s->m[5].type = Celestial;
    s->m[5].id = 5;

    s->m[5].scale = 100000.f;

    s->m[5].cd.v[P][0] = 0.f;
    s->m[5].cd.v[P][1] = 0.f;
    s->m[5].cd.v[P][2] = 0.f;
    s->m[5].cd.v[P][3] = 1.f;

    s->m[5].cull = 1;
    s->m[5].lodlevels = 1;
    // s->m[5].visible = 1;
    s->m[5].mass = 0;
    // s->m[5].material.reflect = 1;
    s->m[5].Q = unitQuat();
    // enWorldMesh(&s->m[5]);
    enworldBbox(&s->m[5]);

    /* ######################################################################################################## */
    memcpy(s->m[6].name, "cubeflat", sizeof("cubeflat"));
    loadMaterial(&s->m[6].material, "gold");
    loadMesh(&s->m[6], s->m[6].name, 1);
    loadBboxVectors(&s->m[6], "objfiles/cubeflat/bbox_cubeflat");
    loadBboxFaces(&s->m[6], "objfiles/cubeflat/lod1");

    s->m[6].type = Camera;
    s->m[6].id = 6;

    s->m[6].scale = 100.f;
    
    vec4f camera[4] = {
        { 1000.0f, 0.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f }
    };
    memcpy(s->m[6].cd.v, camera, 64);

    s->m[6].cull = 1;
    s->m[6].lodlevels = 1;
    // s->m[6].visible = 1;
    s->m[6].weight = 82 * 9.81;
    s->m[6].mass = s->m[6].weight / 9.81;
    s->m[6].fr_coef = 0.5;
    // s->m[6].material.reflect = 1;
    s->m[6].Q = unitQuat();
    // enWorldMesh(&s->m[6]);
    enworldBbox(&s->m[6]);

    /* ######################################################################################################## */
    memcpy(s->m[7].name, "cubeflat", sizeof("cubeflat"));
    loadMaterial(&s->m[7].material, "sunlight");
    loadMesh(&s->m[7], s->m[7].name, 1);
    loadBboxVectors(&s->m[7], "objfiles/cubeflat/bbox_cubeflat");
    loadBboxFaces(&s->m[7], "objfiles/cubeflat/lod1");

    s->m[7].type = Celestial;
    s->m[7].id = 7;

    s->m[7].scale = 1.f;
    
    vec4f lightSource[4] = {
        { 0.f, 10000.0f, 0.f, 1.f },
        { 1.f, 0.f, 0.f, 0.f },
        { 0.f, 0.f, 1.f, 0.f },
        { 0.f, -1.f, 0.f, 0.f }
    };
    memcpy(s->m[7].cd.v, lightSource, 64);

    s->m[7].cull = 1;
    s->m[7].lodlevels = 1;
    // s->m[7].visible = 1;
    s->m[7].mass = 0;
    s->m[7].fr_coef = 0.5;
    // s->m[7].material.reflect = 1;
    s->m[7].Q = unitQuat();
    // enWorldMesh(&s->m[7]);
    enworldBbox(&s->m[7]);

    /* ######################################################################################################## */
    // for (int i = 5; i < MESH_NUM; i++) {
    //     createCube(&s->m[i]);
    //     loadMaterial(&s->m[i].material, "jade");

    //     s->m[i].type = MovingObject;
    //     s->m[i].id = i;
        
    //     s->m[i].scale = 3.f;
    //     s->m[i].cd.v[P][0] = (float)rand() / (float)(RAND_MAX / 400);
    //     s->m[i].cd.v[P][1] = (float)rand() / (float)(RAND_MAX / 100);
    //     s->m[i].cd.v[P][2] = (float)rand() / (float)(RAND_MAX / 400);
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
    }
    free(s->m);
}


