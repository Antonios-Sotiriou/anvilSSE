#include "headers/world_objects.h"

/* This function is responsible to position the objects in world space. */
const void posWorldObjects(Scene *s) {
    Mat4x4 sclMatrix, trMatrix, posMatrix;

    /* ######################################################################################################## */
    s->m[0] = loadmesh("objfiles/terrain.obj");
    // createCube(&s->m[0]);
    // viewFrustum(&s->m[0]);
    sclMatrix = scaleMatrix(1000.0f);
    trMatrix = translationMatrix(0.0f, 0.0f, 0.0f);
    posMatrix = mxm(sclMatrix, trMatrix);

    s->m[0].material = loadmaterial("pearl");
    loadtexture(&s->m[0]);

    s->m[0].pivot = trMatrix.m[3];
    s->m[0].cull = 1;

    s->m[0].v = setvecsarrayxm(s->m[0].v, s->m[0].v_indexes, posMatrix);
    s->m[0].n = setvecsarrayxm(s->m[0].n, s->m[0].n_indexes, posMatrix);

    /* ######################################################################################################## */
    s->m[1] = loadmesh("objfiles/cube.obj");
    // createCube(&s->m[1]);
    sclMatrix = scaleMatrix(100.0f);
    trMatrix = translationMatrix(0.0f, 150.0f, 0.0f);
    posMatrix = mxm(sclMatrix, trMatrix);

    s->m[1].material = loadmaterial("earth");
    loadtexture(&s->m[1]);

    s->m[1].pivot = trMatrix.m[3];
    s->m[1].cull = 1;

    s->m[1].v = setvecsarrayxm(s->m[1].v, s->m[1].v_indexes, posMatrix);
    s->m[1].n = setvecsarrayxm(s->m[1].n, s->m[1].n_indexes, posMatrix);

    /* ######################################################################################################## */
    s->m[2] = loadmesh("objfiles/spacedom.obj");
    // createCube(&s->m[2]);
    sclMatrix = scaleMatrix(10.0f);
    trMatrix = translationMatrix(0.0f, 100.0f, 0.0f);
    posMatrix = mxm(sclMatrix, trMatrix);

    s->m[2].material = loadmaterial("light");
    loadtexture(&s->m[2]);

    s->m[2].pivot = trMatrix.m[3];
    s->m[2].cull = 1;

    s->m[2].v = setvecsarrayxm(s->m[2].v, s->m[2].v_indexes, posMatrix);
    s->m[2].n = setvecsarrayxm(s->m[2].n, s->m[2].n_indexes, posMatrix);
}
/* Loads the appropriate Material with given name and importand Material infos. */
const Material loadmaterial(const char name[]) {
    FILE *fp;
    fp = fopen("tables/materials.dat", "rb");
    Material mat = { 0 };
    if (!fp){
        fclose(fp);
        fprintf(stderr, "Could not open material files < tables/%s >! loadmaterial() -- fopen().\n", name);
    } else {
        while (!feof(fp)) {

            fread(&mat, sizeof(Material), 1, fp);

            if ( strncmp(mat.name, name, strlen(name)) == 0 ) {
                fclose(fp);
                fprintf(stderr, "Material < %s > found... Loading!\n", name);
                return mat;
            }
        }
    }
    fclose(fp);
    fprintf(stderr, "Material < %s > not found!\n", name);
    return mat;
}
/* Loads the appropriate Textures and importand Texture infos. */
const void loadtexture(Mesh *m) {
    if (!m->material.tex_levels)
        return;

    BMP_Header bmp_header;
    BMP_Info info;

    /* Free the previous allocated texture if exists. */
    if (m->material.texture)
        free(m->material.texture);

    size_t len = 15 + (strlen(m->material.name) * 2) + strlen(m->material.texlvl[m->lvlofdetail]);
    char texpath[len];
    snprintf(texpath, len, "textures/%s/%s%s.bmp", m->material.name, m->material.name, m->material.texlvl[m->lvlofdetail]);

    FILE *fp;
    fp = fopen(texpath, "rb");

    if (!fp){
        fclose(fp);
        fprintf(stderr, "Could not open file < %s >! loadtexture() -- fopen().\n", texpath);
    } else {
        fread(&bmp_header, sizeof(BMP_Header), 1, fp);
        fseek(fp, 14, SEEK_SET);
        fread(&info, sizeof(BMP_Info), 1, fp);
        fseek(fp, (14 + info.Size), SEEK_SET);

        /* Subtract 1 from Texture width and height because counting starts from 0; */
        m->material.texture_height = info.Height;
        m->material.texture_width = info.Width;
        const int texSize = info.Height * info.Width;

        m->material.texture = malloc(texSize * 4);
        if (!m->material.texture)
            fprintf(stderr, "Could not allocate memmory for texture: %s. loadtexture()\n", texpath);

        for (int i = (texSize - 1); i >= 0; i--) {
            fread(&m->material.texture[i], 3, 1, fp);
            m->material.texture[i][3] = (unsigned char)255;
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
        free(s->m[i].material.texture);
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

    size_t vsize = sizeof(vec4f) * b.v_indexes;
    size_t nsize = sizeof(vec4f) * b.n_indexes;
    size_t tsize = sizeof(vec2f) * b.t_indexes;
    size_t fsize = sizeof(face) * b.f_indexes;

    a->v = malloc(vsize);
    memcpy(a->v, b.v, vsize);

    a->n = malloc(nsize);
    memcpy(a->n, b.n, nsize);

    a->t = malloc(tsize);
    memcpy(a->t, b.t, tsize);

    a->f = malloc(fsize);
    memcpy(a->f, b.f, fsize);
}


