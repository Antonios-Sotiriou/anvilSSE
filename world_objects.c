#include "headers/world_objects.h"

extern float AmbientStrength, SpecularStrength;
// const static void loadTexture(Mesh *c);

/* This function is responsible to position the objects in world space. */
const void posWorldObjects(Scene *s) {
    Mat4x4 sclMatrix, trMatrix, posMatrix;
    Material mat = {
        .texture_file = "textures/stones.bmp",
        .basecolor = { 1.0f, 0.8f, 0.0f, 0.0f },
        .ambient = { 0.24725f, 0.1995f, 0.0745f, 0.0f },
        .specular = { 0.628281f, 0.555802f, 0.366065f, 0.0f },
        .diffuse = { 0.75164f, 0.60648f, 0.22648f, 0.0f },
        .shinniness = 0.4 * 128,
        .reflect = 0
    };

    s->m[0] = loadmesh("objfiles/terrain.obj");
    // createCube(&s->m[0]);
    sclMatrix = scaleMatrix(1000.0f);
    trMatrix = translationMatrix(0.0f, 0.0f, 0.0f);
    posMatrix = mxm(sclMatrix, trMatrix);

    s->m[0].material = mat;
    loadTexture(&s->m[0]);

    s->m[0].pivot = trMatrix.m[3];

    s->m[0].v = setvecsarrayxm(s->m[0].v, s->m[0].v_indexes, posMatrix);
    s->m[0].n = setvecsarrayxm(s->m[0].n, s->m[0].n_indexes, posMatrix);

    /* ######################################################################################################## */
    s->m[1] = loadmesh("objfiles/cube.obj");
    sclMatrix = scaleMatrix(10.0f);
    trMatrix = translationMatrix(0.0f, 15.0f, 0.0f);
    posMatrix = mxm(sclMatrix, trMatrix);

    s->m[1].material = mat;
    memcpy(&s->m[1].material.texture_file, "textures/Earth.bmp", 19);
    loadTexture(&s->m[1]);
    s->m[1].material.reflect = 1;

    s->m[1].pivot = trMatrix.m[3];

    s->m[1].v = setvecsarrayxm(s->m[1].v, s->m[1].v_indexes, posMatrix);
    s->m[1].n = setvecsarrayxm(s->m[1].n, s->m[1].n_indexes, posMatrix);

    /* ######################################################################################################## */
    s->m[2] = loadmesh("objfiles/spacedom.obj");
    sclMatrix = scaleMatrix(10.0f);
    trMatrix = translationMatrix(0.0f, 100.0f, 0.0f);
    posMatrix = mxm(sclMatrix, trMatrix);

    s->m[2].material = mat;
    memcpy(&s->m[2].material.texture_file, "textures/light.bmp", 19);
    loadTexture(&s->m[2]);
    s->m[2].material.reflect = 1;

    s->m[2].pivot = trMatrix.m[3];

    s->m[2].v = setvecsarrayxm(s->m[2].v, s->m[2].v_indexes, posMatrix);
    s->m[2].n = setvecsarrayxm(s->m[2].n, s->m[2].n_indexes, posMatrix);
}
/* Loads the appropriate Textures and importand Texture infos. */
const void loadTexture(Mesh *c) {
    BMP_Header bmp_header;
    BMP_Info info;

    FILE *fp;
    fp = fopen(c->material.texture_file, "rb");

    if (fp == NULL){
        fclose(fp);
        fprintf(stderr, "Could not open file < %s >! loadTexture() -- fopen().\n", c->material.texture_file);
    } else {
        fread(&bmp_header, sizeof(BMP_Header), 1, fp);
        fseek(fp, 14, SEEK_SET);
        fread(&info, sizeof(BMP_Info), 1, fp);
        fseek(fp, (14 + info.Size), SEEK_SET);

        /* Subtract 1 from Texture width and height because counting starts from 0; */
        c->material.texture_height = info.Height;
        c->material.texture_width = info.Width;
        const int texSize = info.Height * info.Width;

        c->material.texture = malloc(texSize * 4);
        if (!c->material.texture)
            fprintf(stderr, "Could not allocate memmory for texture: %s. loadTexture()\n", c->material.texture_file);

        for (int i = (texSize - 1); i >= 0; i--) {
            fread(&c->material.texture[i], 3, 1, fp);
            c->material.texture[i][3] = (unsigned char)255;
        }
    }
    fclose(fp);
}
/* Teams all objects of the the world in a scene for further procesing. */
const void createScene(Scene *s) {
    s->m = malloc(sizeof(Mesh) * 3);
    s->m_indexes = 3;
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
    free(c->material.texture);
}
/* Initializing Mesh a from Mesh b. */
const void initMesh(Mesh *a, const Mesh b) {
    *a = b;

    size_t tsize = sizeof(vec2f) * b.t_indexes;
    size_t fsize = sizeof(face) * b.f_indexes;
    /* To get the proper size, add 1 to width and height, because we subtracted when created the Texture. */
    size_t texsize = b.material.texture_height * b.material.texture_width * 4;

    a->t = malloc(tsize);
    memcpy(a->t, b.t, tsize);

    a->f = malloc(fsize);
    memcpy(a->f, b.f, fsize);

    a->material.texture = malloc(texsize);
    memcpy(a->material.texture, b.material.texture, texsize);
}


