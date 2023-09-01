#include "headers/world_objects.h"

extern float AmbientStrength, SpecularStrength;
const static void loadTexture(Mesh *c);

/* This function is responsible to position the objects in world space. */
const void posWorldObjects(Scene *s) {
    Mesh cube = loadmesh("objfiles/terrain.obj");
    // createCube(&cube);
    Mat4x4 sclMatrix, trMatrix, posMatrix;
    
    sclMatrix = scaleMatrix(1000.0);
    trMatrix = translationMatrix(0.0, 0.0, 0.0);
    posMatrix = mxm(sclMatrix, trMatrix);

    Material mat = {
        .texture_file = "textures/stones.bmp",
        .basecolor = { 1.0f, 0.8f, 0.0f, 0.0f },
        .ambient = { 0.24725f, 0.1995f, 0.0745f, 0.0f },
        .specular = { 0.628281f, 0.555802f, 0.366065f, 0.0f },
        .diffuse = { 0.75164f, 0.60648f, 0.22648f, 0.0f },
        .shinniness = 0.4 * 128,
        .reflect = 0
    };

    cube.material = mat;
    loadTexture(&cube);

    initMesh(&s->m[0], cube);
    s->m[0].v = meshxm(cube.v, cube.v_indexes, posMatrix);
    s->m[0].n = meshxm(cube.n, cube.n_indexes, posMatrix);

    releaseMesh(&cube);
    /* ######################################################################################################## */
    Mesh jupiter = loadmesh("objfiles/earth.obj");
    sclMatrix = scaleMatrix(10.0);
    trMatrix = translationMatrix(0.0, 0.0, 80.0);
    posMatrix = mxm(sclMatrix, trMatrix);

    jupiter.material = mat;
    loadTexture(&jupiter);
    jupiter.material.reflect = 1;

    initMesh(&s->m[1], jupiter);
    s->m[1].v = meshxm(jupiter.v, jupiter.v_indexes, posMatrix);
    s->m[1].n = meshxm(jupiter.n, jupiter.n_indexes, posMatrix);

    releaseMesh(&jupiter);
}
/* Loads the appropriate Textures and importand Texture infos. */
const static void loadTexture(Mesh *c) {

    BMP_Header bmp_header;
    BMP_Info texture;

    FILE *fp;
    fp = fopen(c->material.texture_file, "rb");

    if (fp == NULL){
        fclose(fp);
        fprintf(stderr, "Could not open file < %s >! loadTexture() -- fopen().\n", c->material.texture_file);
    } else {
        fread(&bmp_header, sizeof(BMP_Header), 1, fp);
        fseek(fp, 14, SEEK_SET);
        fread(&texture, sizeof(BMP_Info), 1, fp);
        fseek(fp, (14 + texture.Size), SEEK_SET);

        c->material.texture_height = texture.Height;
        c->material.texture_width = texture.Width;
        const int texSize = texture.Height * texture.Width;
        c->material.texture = malloc(texSize * 4);

        for (int i = texSize; i >= 0; i--) {
            fread(&c->material.texture[i], 3, 1, fp);
            c->material.texture[i][3] = (unsigned char)255;
        }
    }
    fclose(fp);
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
    free(c->material.texture);
}
/* Initializing Mesh a from Mesh b. */
const void initMesh(Mesh *a, const Mesh b) {
    *a = b;
    // size_t nsize = sizeof(vec4f) * b.n_indexes;
    size_t tsize = sizeof(vec4f) * b.t_indexes;
    size_t fsize = sizeof(face) * b.f_indexes;
    size_t texsize = b.material.texture_height * b.material.texture_height * 4;

    // a->n = malloc(nsize);
    // memcpy(a->n, b.n, nsize);

    a->t = malloc(tsize);
    memcpy(a->t, b.t, tsize);

    a->f = malloc(fsize);
    memcpy(a->f, b.f, fsize);

    a->material.texture = malloc(texsize);
    memcpy(a->material.texture, b.material.texture, texsize);
}


