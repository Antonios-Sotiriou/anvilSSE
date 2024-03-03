#include "headers/test_shapes.h"
#include "headers/logging.h"
const void createCube(Mesh *c) {
    const size_t face_size = sizeof(unsigned int) * 108;
    c->v = malloc(16 * 8);
    c->n = malloc(16 * 8);
    c->t = malloc(8 * 14);
    c->f = malloc(face_size);

    vec4f vectors[8] = {
        { -1.000000, -1.000000, -1.000000, 1.0 },
        { -1.000000, 1.000000, -1.000000, 1.0 },
        { -1.000000, -1.000000, 1.000000, 1.0 },
        { -1.000000, 1.000000, 1.000000, 1.0 },
        { 1.000000, -1.000000, -1.000000, 1.0 },
        { 1.000000, 1.000000, -1.000000, 1.0 },
        { 1.000000, -1.000000, 1.000000, 1.0 },
        { 1.000000, 1.000000, 1.000000, 1.0 }
    };
    vec4f normals[8] = {
        { -0.5774, -0.5774, 0.5774, 0.0 },
        { -0.5774, -0.5774, -0.5774, 0.0 },
        { 0.5774, -0.5774, -0.5774, 0.0 },
        { 0.5774, 0.5774, 0.5774, 0.0 },
        { -0.5774, 0.5774, 0.5774, 0.0 },
        { 0.5774, 0.5774, -0.5774, 0.0 },
        { 0.5774, -0.5774, 0.5774, 0.0 },
        { -0.5774, 0.5774, -0.5774, 0.0 }
    };
    vec2f textures[14] = {
        { 0.250869, 0.999111 },
        { 0.000272, 0.664772 },
        { 0.999470, 0.666021 },
        { 0.000071, 0.334027 },
        { 0.250490, 0.000967 },
        { 0.999545, 0.334644 },
        { 0.251146, 0.664645 },
        { 0.250069, 0.333989 },
        { 0.499504, 0.999136 },
        { 0.749957, 0.666010 },
        { 0.499506, 0.000737 },
        { 0.749957, 0.334637 },
        { 0.499504, 0.666010 },
        { 0.499493, 0.334624 }
    };
    unsigned int faces[108] = {
        2, 6, 0, 0, 0, 1, 4, 8, 2,
        7, 13, 3, 3, 7, 4, 2, 6, 0,
        5, 11, 5, 7, 13, 3, 6, 12, 6,
        7, 13, 3, 5, 10, 5, 1, 4, 7,
        3, 7, 4, 1, 3, 7, 0, 1, 1,
        1, 5, 7, 5, 11, 5, 4, 9, 2,
        2, 6, 0, 4, 8, 2, 6, 12, 6,
        7, 13, 3, 2, 6, 0, 6, 12, 6,
        5, 11, 5, 6, 12, 6, 4, 9, 2,
        7, 13, 3, 1, 4, 7, 3, 7, 4,
        3, 7, 4, 0, 1, 1, 2, 6, 0,
        1, 5, 7, 4, 9, 2, 0, 2, 1
    };
    c->v_indexes = 8;
    c->n_indexes = 8;
    c->t_indexes = 14;
    c->f_indexes = 108;

    memcpy(c->v, vectors, 16 * 8);
    memcpy(c->n, normals, 16 * 8);
    memcpy(c->t, textures, 8 * 14);
    memcpy(c->f, faces, face_size);
}
const void createskybox(Mesh *c) {
    const size_t face_size = sizeof(unsigned int) * 108;
    c->v = malloc(16 * 8);
    c->n = malloc(16 * 8);
    c->t = malloc(8 * 14);
    c->f = malloc(face_size);

    vec4f vectors[8] = {
        { 1.000000f, 1.000000f, -1.000000f, 1.f },
        { 1.000000f, -1.000000f, -1.000000f, 1.f },
        { 1.000000f, 1.000000f, 1.000000f, 1.f },
        { 1.000000f, -1.000000f, 1.000000f, 1.f },
        { -1.000000f, 1.000000f, -1.000000f, 1.f },
        { -1.000000f, -1.000000f, -1.000000f, 1.f },
        { -1.000000f, 1.000000f, 1.000000f, 1.f },
        { -1.000000f, -1.000000f, 1.000000f, 1.f }
    };
    vec4f normals[8] = {
        { -0.5774f, -0.5774f, -0.5774f, 0.f },
        { 0.5774f, -0.5774f, 0.5774f, 0.f },
        { -0.5774f, -0.5774f, 0.5774f, 0.f },
        { 0.5774f, 0.5774f, -0.5774f, 0.f },
        { -0.5774f, 0.5774f, -0.5774f, 0.f },
        { 0.5774f, 0.5774f, 0.5774f, 0.f },
        { 0.5774f, -0.5774f, -0.5774f, 0.f },
        { -0.5774f, 0.5774f, 0.5774f, 0.f }
    };
    vec2f textures[14] = {
        { 0.250869, 0.999111 },
        { 0.000272, 0.664772 },
        { 0.999470, 0.666021 },
        { 0.000071, 0.334027 },
        { 0.250490, 0.000967 },
        { 0.999545, 0.334644 },
        { 0.251146, 0.664645 },
        { 0.250069, 0.333989 },
        { 0.499504, 0.999136 },
        { 0.749957, 0.666010 },
        { 0.499506, 0.000737 },
        { 0.749957, 0.334637 },
        { 0.499504, 0.666010 },
        { 0.499493, 0.334624 }
    };
    unsigned int faces[108] = {
        2, 6, 0, 4, 8, 1, 0, 0, 2,
        7, 13, 3, 2, 6, 0, 3, 7, 4,
        5, 11, 5, 6, 12, 6, 7, 13, 3,
        7, 13, 3, 1, 4, 7, 5, 10, 5,
        3, 7, 4, 0, 1, 2, 1, 3, 7,
        1, 5, 7, 4, 9, 1, 5, 11, 5,
        2, 6, 0, 6, 12, 6, 4, 8, 1,
        7, 13, 3, 6, 12, 6, 2, 6, 0,
        5, 11, 5, 4, 9, 1, 6, 12, 6,
        7, 13, 3, 3, 7, 4, 1, 4, 7,
        3, 7, 4, 2, 6, 0, 0, 1, 2,
        1, 5, 7, 0, 2, 2, 4, 9, 1
    };
    c->v_indexes = 8;
    c->n_indexes = 8;
    c->t_indexes = 14;
    c->f_indexes = 108;

    memcpy(c->v, vectors, 16 * 8);
    memcpy(c->n, normals, 16 * 8);
    memcpy(c->t, textures, 8 * 14);
    memcpy(c->f, faces, face_size);
}
const void createPlane(Mesh *c) {
    const size_t face_size = sizeof(unsigned int) * 18;
    c->v = malloc(16 * 4);
    c->n = malloc(16 * 4);
    c->t = malloc(8 * 4);
    c->f = malloc(face_size);

    vec4f vectors[4] = {
        { -1.000000f, 0.000000f, -1.000000f, 1.f },
        { -1.000000f, 0.000000f, 1.000000f, 1.f },
        { 1.000000f, 0.000000f, 1.000000f, 1.f },
        { 1.000000f, 0.000000f, -1.000000f, 1.f },
    };
    vec4f normals[4] = {
        { 0.f, 1.0f, 0.f, 0.f },
        { 0.f, 1.0f, 0.f, 0.f },
        { 0.f, 1.0f, 0.f, 0.f },
        { 0.f, 1.0f, 0.f, 0.f },
    };
    vec2f textures[4] = {
        { 0.f, 0.f },
        { 0.f, 1.f },
        { 1.f, 1.f },
        { 1.f, 0.f },
    };
    unsigned int faces[18] = {
        0, 0, 0, 1, 1, 1, 2, 2, 2,
        0, 0, 0, 2, 2, 2, 3, 3, 3
    };
    c->v_indexes = 4;
    c->n_indexes = 4;
    c->t_indexes = 4;
    c->f_indexes = 18;

    memcpy(c->v, vectors, 16 * 4);
    memcpy(c->n, normals, 16 * 4);
    memcpy(c->t, textures, 8 * 4);
    memcpy(c->f, faces, face_size);
}
/* Rows and colums here are given in Quads. Consider that each quad consists of 4 vertices. */
const void createGrid(Mesh *c, int vrows, int vcols) {
    if ( vrows == 0 || vcols == 0 ) {
        fprintf(stderr, "Zero value for %s. createGrid() --> ERROR 1\n", vrows == 0 ? "vrows" : vcols == 0 ? "vcols" : "input");
        exit(1);
    }

    vrows = (vrows % 2 == 0) ? vrows + 1 : vrows;
    vcols = (vcols % 2 == 0) ? vcols + 1 : vcols;

    /* Emvadon vertices. Must be 1 more than given from user in both directions to corect handle all cases with faces and quads. */
    const int emvadon = vrows * vcols;

    /* Quads. */
    const int quad_vrows = vrows == 1 ? 1 : vrows - 1;
    const int quad_vcols = vcols == 1 ? 1 : vcols - 1;
    const int quads = quad_vrows * quad_vcols;

    /* Faces. */
    const int faces_per_row = quad_vrows * 2;
    const int num_of_faces = quads * 2 * 9;

    c->v = calloc(emvadon, 16);
    c->n = calloc(emvadon, 16);
    c->t = calloc(emvadon, 8);
    c->f = calloc(num_of_faces, 4);

    /* Vectors initialization. ############################## */ 
    float step_x = (2.f / quad_vrows);
    float step_z = (2.f / quad_vcols);
    float start_x = -1.f;
    float start_z = -1.f;

    float x_step_cache = start_x;
    float z_step_cache = start_z;

    int vrows_count = vrows;

    for (int x = 0; x < emvadon; x++) {

        if ( x == vrows_count ) {
            x_step_cache = start_x;
            z_step_cache += step_z;

            vrows_count += vrows;
        }

        c->v[x][0] += x_step_cache;
        // c->v[x][1] = (float)rand() / (float)(RAND_MAX / 0.09f);
        c->v[x][1] = 0;
        c->v[x][2] = z_step_cache;
        c->v[x][3] = 1.f;

        x_step_cache += step_x;
    }

    /* Normals initialization. ############################## */
    vec4f normal = { 0.f, 1.f, 0.f, 0.f };
    for (int x = 0; x < emvadon; x++) {
            c->n[x] = normal;
    }

    /* Textors initialization. ############################## */
    float step_tu = 1.f / quad_vrows;
    float step_tv = 1.f / quad_vcols;
    float start_tu = 0.f;
    float start_tv = 0.f;
    float tu_step_cache = start_tu;
    float tv_step_cache = start_tv;

    int tx_count = vrows;
    for (int x = 0; x < emvadon; x++) {

        if ( x == tx_count ) {
            tu_step_cache = start_tu;
            tv_step_cache += step_tv;

            tx_count += vrows;
        }
        c->t[x][0] = tu_step_cache;
        c->t[x][1] = tv_step_cache;

        tu_step_cache += step_tu;
    }

    /* faces initialization. ############################## */
    int face_1_0 = 0;
    int face_1_1 = vrows;
    int face_1_2 =  vrows + 1;
    int face_counter = 0;

    for (int x = 0; x < num_of_faces; x += 18) {

        if (face_counter == faces_per_row) {
            face_1_0 += 1;
            face_1_1 += 1;
            face_1_2 += 1;

            face_counter = 0;
        }

        /* Face 1st Up. */
        c->f[x] = face_1_0;
        c->f[x + 1] = face_1_0;

        c->f[x + 3] = face_1_1;
        c->f[x + 4] = face_1_1;

        c->f[x + 6] = face_1_2;
        c->f[x + 7] = face_1_2;

        /* Face 2nd Down. */
        c->f[x + 9] = face_1_0;
        c->f[x + 10] = face_1_0;

        c->f[x + 12] = face_1_2;
        c->f[x + 13] = face_1_2;

        c->f[x + 15] = face_1_0 + 1;
        c->f[x + 16] = face_1_0 + 1;

        face_1_0++;
        face_1_1++;
        face_1_2++;

        face_counter += 2;
    }

    c->v_indexes = emvadon;
    c->n_indexes = emvadon;
    c->t_indexes = emvadon;
    c->f_indexes = num_of_faces;
}
const void createTerrain(Mesh *m, const char path[]) {
    BMP bmp;
    int emvadon;
    char *data;

    FILE *fp;
    fp = fopen(path, "rb");

    if (!fp){
        fprintf(stderr, "Could not open file < %s >! readHeightmap() -- fopen().\n", path);
        return;
    } else {
        fread(&bmp.header, sizeof(BMP_Header), 1, fp);
        fseek(fp, 14, SEEK_SET);
        fread(&bmp.info, sizeof(BMP_Info), 1, fp);
        fseek(fp, bmp.header.Res2, SEEK_SET);

        emvadon = bmp.info.Height * bmp.info.Width;
        if (!emvadon) {
            fprintf(stderr, "Zero value for Height map Size: %d. createTerrain() --> ERROR 1\n", emvadon);
            return;
        }
        data = malloc(emvadon);
        fread(data, emvadon, 1, fp);
    }
    fclose(fp);

    /* Populate the gloabal TerrainInfo struct with infos. */
    tf.vecWidth = bmp.info.Width;
    tf.vecHeight = bmp.info.Height;
    tf.quadCols = tf.vecWidth - 1;
    tf.quadRows = tf.vecHeight - 1;
    tf.quadsArea = tf.quadRows * tf.quadCols;
    tf.quads = calloc(tf.quadsArea, sizeof(Quad));

    /* Quads. */
    const int quad_vcols = bmp.info.Width - 1;
    const int quad_vrows = bmp.info.Height - 1;
    const int quads = quad_vrows * quad_vcols;
    if (!quads) {
        fprintf(stderr, "Zero value for Quads: %d. createTerrain() --> ERROR 1\n", emvadon);
        exit(1);
    }

    /* Faces. */
    const int faces_per_row = quad_vrows * 2;
    const int num_of_faces = quads * 2 * 9;

    m->v = calloc(emvadon, 16);
    m->n = calloc(emvadon, 16);
    m->t = calloc(emvadon, 8);
    m->f = calloc(num_of_faces, 4);

    /* Vectors initialization. ############################## */ 
    float step_x = 1.f / bmp.info.Width;
    float step_z = 1.f / bmp.info.Height;
    float start_x = (step_x * (bmp.info.Width * 0.5)) - 1;
    float start_z = (step_x * (bmp.info.Height * 0.5)) - 1;

    float x_step_cache = start_x;
    float z_step_cache = start_z;

    int vcols_count = bmp.info.Width;

    for (int x = 0; x < emvadon; x++) {

        if ( x == vcols_count ) {
            x_step_cache = start_x;
            z_step_cache += step_z;

            vcols_count += bmp.info.Width;
        }

        m->v[x][0] += x_step_cache;
        // c->v[x][1] = (float)rand() / (float)(RAND_MAX / 0.09f);
        m->v[x][1] = 0;//data[x] / 255.f;
        m->v[x][2] = z_step_cache;
        m->v[x][3] = 1.f;

        x_step_cache += step_x;
    }
    free(data);

    /* Normals initialization. ############################## */
    vec4f normal = { 0.f, 1.f, 0.f, 0.f };
    for (int x = 0; x < emvadon; x++) {
            m->n[x] = normal;
    }

    /* Textors initialization. ############################## */
    float step_tu = 1.f / quad_vrows;
    float step_tv = 1.f / quad_vcols;
    float start_tu = 0.f;
    float start_tv = 0.f;
    float tu_step_cache = start_tu;
    float tv_step_cache = start_tv;

    int tx_count = bmp.info.Height;
    for (int x = 0; x < emvadon; x++) {

        if ( x == tx_count ) {
            tu_step_cache = start_tu;
            tv_step_cache += step_tv;

            tx_count += bmp.info.Height;
        }
        m->t[x][0] = tu_step_cache;
        m->t[x][1] = tv_step_cache;

        tu_step_cache += step_tu;
    }

    /* faces initialization. ############################## */
    int face_1_0 = 0;
    int face_1_1 = bmp.info.Height;
    int face_1_2 =  bmp.info.Height + 1;
    int face_counter = 0;

    for (int x = 0; x < num_of_faces; x += 18) {

        if (face_counter == faces_per_row) {
            face_1_0 += 1;
            face_1_1 += 1;
            face_1_2 += 1;

            face_counter = 0;
        }

        /* Face 1st Up. */
        m->f[x] = face_1_0;
        m->f[x + 1] = face_1_0;

        m->f[x + 3] = face_1_1;
        m->f[x + 4] = face_1_1;

        m->f[x + 6] = face_1_2;
        m->f[x + 7] = face_1_2;

        /* Face 2nd Down. */
        m->f[x + 9] = face_1_0;
        m->f[x + 10] = face_1_0;

        m->f[x + 12] = face_1_2;
        m->f[x + 13] = face_1_2;

        m->f[x + 15] = face_1_0 + 1;
        m->f[x + 16] = face_1_0 + 1;

        face_1_0++;
        face_1_1++;
        face_1_2++;

        face_counter += 2;
    }

    m->v_indexes = emvadon;
    m->n_indexes = emvadon;
    m->t_indexes = emvadon;
    m->f_indexes = num_of_faces;
}


