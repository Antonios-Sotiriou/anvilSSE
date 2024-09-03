#include "../../headers/components/test_shapes.h"

const void createCube(Mesh *m) {
    vec4f v[8] = {
        { -1.000000, -1.000000, -1.000000, 1.0 },
        { -1.000000, 1.000000, -1.000000, 1.0 },
        { -1.000000, -1.000000, 1.000000, 1.0 },
        { -1.000000, 1.000000, 1.000000, 1.0 },
        { 1.000000, -1.000000, -1.000000, 1.0 },
        { 1.000000, 1.000000, -1.000000, 1.0 },
        { 1.000000, -1.000000, 1.000000, 1.0 },
        { 1.000000, 1.000000, 1.000000, 1.0 }
    };
    vec4f n[8] = {
        { -0.5774, -0.5774, 0.5774, 0.0 },
        { -0.5774, -0.5774, -0.5774, 0.0 },
        { 0.5774, -0.5774, -0.5774, 0.0 },
        { 0.5774, 0.5774, 0.5774, 0.0 },
        { -0.5774, 0.5774, 0.5774, 0.0 },
        { 0.5774, 0.5774, -0.5774, 0.0 },
        { 0.5774, -0.5774, 0.5774, 0.0 },
        { -0.5774, 0.5774, -0.5774, 0.0 }
    };
    vec2f t[14] = {
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
    int f[108] = {
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

    m->f_indexes = 108 / 9;
    m->f = malloc(sizeof(face) * m->f_indexes);

    int index = 0;
    for (int i = 0; i < 108; i += 9) {
        m->f[index].v[0] = v[f[i]];
        m->f[index].v[1] = v[f[i + 3]];
        m->f[index].v[2] = v[f[i + 6]];

        m->f[index].vt[0] = t[f[i + 1]];
        m->f[index].vt[1] = t[f[i + 4]];
        m->f[index].vt[2] = t[f[i + 7]];

        m->f[index].vn[0] = n[f[i + 2]];
        m->f[index].vn[1] = n[f[i + 5]];
        m->f[index].vn[2] = n[f[i + 8]];
        index++;
    }

    const int vecs_num = m->f_indexes * 3;
    m->vba = malloc(vecs_num * 32);
    m->vba_indexes = vecs_num;

    int el_index = 0;
    float chunk[8];
    for (int i = 0; i < m->f_indexes; i++) {
        for (int j = 0; j < 3; j++) {

            chunk[0] = m->f[i].v[j][0];
            chunk[1] = m->f[i].v[j][1];
            chunk[2] = m->f[i].v[j][2];
            chunk[3] = m->f[i].vt[j][0];
            chunk[4] = m->f[i].vt[j][1];
            chunk[5] = m->f[i].vn[j][0];
            chunk[6] = m->f[i].vn[j][1];
            chunk[7] = m->f[i].vn[j][2];

            memcpy(&m->vba[el_index], &chunk, 32);
            el_index++;
        }
    }
}
const void createskybox(Mesh *m) {
    vec4f v[8] = {
        { 1.000000f, 1.000000f, -1.000000f, 1.f },
        { 1.000000f, -1.000000f, -1.000000f, 1.f },
        { 1.000000f, 1.000000f, 1.000000f, 1.f },
        { 1.000000f, -1.000000f, 1.000000f, 1.f },
        { -1.000000f, 1.000000f, -1.000000f, 1.f },
        { -1.000000f, -1.000000f, -1.000000f, 1.f },
        { -1.000000f, 1.000000f, 1.000000f, 1.f },
        { -1.000000f, -1.000000f, 1.000000f, 1.f }
    };
    vec4f n[8] = {
        { -0.5774f, -0.5774f, -0.5774f, 0.f },
        { 0.5774f, -0.5774f, 0.5774f, 0.f },
        { -0.5774f, -0.5774f, 0.5774f, 0.f },
        { 0.5774f, 0.5774f, -0.5774f, 0.f },
        { -0.5774f, 0.5774f, -0.5774f, 0.f },
        { 0.5774f, 0.5774f, 0.5774f, 0.f },
        { 0.5774f, -0.5774f, -0.5774f, 0.f },
        { -0.5774f, 0.5774f, 0.5774f, 0.f }
    };
    vec2f t[14] = {
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
    unsigned int f[108] = {
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

    m->f_indexes = 108 / 9;
    m->f = malloc(sizeof(face) * m->f_indexes);

    int index = 0;
    for (int i = 0; i < 108; i += 9) {
        m->f[index].v[0] = v[f[i]];
        m->f[index].v[1] = v[f[i + 3]];
        m->f[index].v[2] = v[f[i + 6]];

        m->f[index].vt[0] = t[f[i + 1]];
        m->f[index].vt[1] = t[f[i + 4]];
        m->f[index].vt[2] = t[f[i + 7]];

        m->f[index].vn[0] = n[f[i + 2]];
        m->f[index].vn[1] = n[f[i + 5]];
        m->f[index].vn[2] = n[f[i + 8]];
        index++;
    }

    const int vecs_num = m->f_indexes * 3;
    m->vba = malloc(vecs_num * 32);
    m->vba_indexes = vecs_num;

    int el_index = 0;
    float chunk[8];
    for (int i = 0; i < m->f_indexes; i++) {
        for (int j = 0; j < 3; j++) {

            chunk[0] = m->f[i].v[j][0];
            chunk[1] = m->f[i].v[j][1];
            chunk[2] = m->f[i].v[j][2];
            chunk[3] = m->f[i].vt[j][0];
            chunk[4] = m->f[i].vt[j][1];
            chunk[5] = m->f[i].vn[j][0];
            chunk[6] = m->f[i].vn[j][1];
            chunk[7] = m->f[i].vn[j][2];

            memcpy(&m->vba[el_index], &chunk, 32);
            el_index++;
        }
    }
}
const void createPlane(Mesh *m) {
    vec4f v[4] = {
        { -1.000000f, 0.000000f, -1.000000f, 1.f },
        { -1.000000f, 0.000000f, 1.000000f, 1.f },
        { 1.000000f, 0.000000f, 1.000000f, 1.f },
        { 1.000000f, 0.000000f, -1.000000f, 1.f },
    };
    vec4f n[4] = {
        { 0.f, 1.0f, 0.f, 0.f },
        { 0.f, 1.0f, 0.f, 0.f },
        { 0.f, 1.0f, 0.f, 0.f },
        { 0.f, 1.0f, 0.f, 0.f },
    };
    vec2f t[4] = {
        { 0.f, 0.f },
        { 0.f, 1.f },
        { 1.f, 1.f },
        { 1.f, 0.f },
    };
    unsigned int f[18] = {
        0, 0, 0, 1, 1, 1, 2, 2, 2,
        0, 0, 0, 2, 2, 2, 3, 3, 3
    };

    m->f_indexes = 18 / 9;
    m->f = malloc(sizeof(face) * m->f_indexes);

    int index = 0;
    for (int i = 0; i < 18; i += 9) {
        m->f[index].v[0] = v[f[i]];
        m->f[index].v[1] = v[f[i + 3]];
        m->f[index].v[2] = v[f[i + 6]];

        m->f[index].vt[0] = t[f[i + 1]];
        m->f[index].vt[1] = t[f[i + 4]];
        m->f[index].vt[2] = t[f[i + 7]];

        m->f[index].vn[0] = n[f[i + 2]];
        m->f[index].vn[1] = n[f[i + 5]];
        m->f[index].vn[2] = n[f[i + 8]];
        index++;
    }

    const int vecs_num = m->f_indexes * 3;
    m->vba = malloc(vecs_num * 32);
    m->vba_indexes = vecs_num;

    int el_index = 0;
    float chunk[8];
    for (int i = 0; i < m->f_indexes; i++) {
        for (int j = 0; j < 3; j++) {

            chunk[0] = m->f[i].v[j][0];
            chunk[1] = m->f[i].v[j][1];
            chunk[2] = m->f[i].v[j][2];
            chunk[3] = m->f[i].vt[j][0];
            chunk[4] = m->f[i].vt[j][1];
            chunk[5] = m->f[i].vn[j][0];
            chunk[6] = m->f[i].vn[j][1];
            chunk[7] = m->f[i].vn[j][2];

            memcpy(&m->vba[el_index], &chunk, 32);
            el_index++;
        }
    }
}
/* Rows and colums here are given in Quads. Consider that each quad consists of 4 vertices. */
const void createGrid(Mesh *m, int vrows, int vcols) {
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

    vec4f * v = calloc(emvadon, 16);
    vec4f * n = calloc(emvadon, 16);
    vec2f * t = calloc(emvadon, 8);
    int * f = calloc(num_of_faces, 4);

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

        v[x][0] += x_step_cache;
        // c->v[x][1] = (float)rand() / (float)(RAND_MAX / 0.09f);
        v[x][1] = 0;
        v[x][2] = z_step_cache;
        v[x][3] = 1.f;

        x_step_cache += step_x;
    }

    /* Normals initialization. ############################## */
    vec4f normal = { 0.f, 1.f, 0.f, 0.f };
    for (int x = 0; x < emvadon; x++) {
            n[x] = normal;
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
        t[x][0] = tu_step_cache;
        t[x][1] = tv_step_cache;

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
        f[x] = face_1_0;
        f[x + 1] = face_1_0;

        f[x + 3] = face_1_1;
        f[x + 4] = face_1_1;

        f[x + 6] = face_1_2;
        f[x + 7] = face_1_2;

        /* Face 2nd Down. */
        f[x + 9] = face_1_0;
        f[x + 10] = face_1_0;

        f[x + 12] = face_1_2;
        f[x + 13] = face_1_2;

        f[x + 15] = face_1_0 + 1;
        f[x + 16] = face_1_0 + 1;

        face_1_0++;
        face_1_1++;
        face_1_2++;

        face_counter += 2;
    }

    m->f_indexes = num_of_faces / 9;
    m->f = malloc(sizeof(face) * m->f_indexes);

    int index = 0;
    for (int i = 0; i < num_of_faces; i += 9) {
        m->f[index].v[0] = v[f[i]];
        m->f[index].v[1] = v[f[i + 3]];
        m->f[index].v[2] = v[f[i + 6]];

        m->f[index].vt[0] = t[f[i + 1]];
        m->f[index].vt[1] = t[f[i + 4]];
        m->f[index].vt[2] = t[f[i + 7]];

        m->f[index].vn[0] = n[f[i + 2]];
        m->f[index].vn[1] = n[f[i + 5]];
        m->f[index].vn[2] = n[f[i + 8]];
        index++;
    }

    const int vecs_num = m->f_indexes * 3;
    m->vba = malloc(vecs_num * 32);
    m->vba_indexes = vecs_num;

    int el_index = 0;
    float chunk[8];
    for (int i = 0; i < m->f_indexes; i++) {
        for (int j = 0; j < 3; j++) {

            chunk[0] = m->f[i].v[j][0];
            chunk[1] = m->f[i].v[j][1];
            chunk[2] = m->f[i].v[j][2];
            chunk[3] = m->f[i].vt[j][0];
            chunk[4] = m->f[i].vt[j][1];
            chunk[5] = m->f[i].vn[j][0];
            chunk[6] = m->f[i].vn[j][1];
            chunk[7] = m->f[i].vn[j][2];

            memcpy(&m->vba[el_index], &chunk, 32);
            el_index++;
        }
    }

    free(v);
    free(t);
    free(n);
    free(f);
}


