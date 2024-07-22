#include "../headers/general_functions.h"

const static void checkVisibility(Mesh *m, const int len, Mat4x4 vm, const int viewProj);

/* Swaping two variables a and b of any type with size. */
const void swap(void *a, void *b, unsigned long size) {
    void *temp = malloc(size);
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
    free(temp);
}
/* Returns the given value in radians. */
const float radians(const float value) {
    return value * (3.14159 / 180.0);
}
/* Finds the minX, maxX, minY, maxY, minZ maxZ values of given vectors array. AKA (bounding box). */
const DimensionsLimits getDimensionsLimits(vec4f va[], const int len) {
    DimensionsLimits dl = {
        .min = dl.max = va[0],
    };

    for (int i = 1; i < len; i++) {
        /* Get min and max x values. */
        if ( va[i][0] <= dl.min[0]) {
            dl.min[0] = va[i][0];
        } else if ( va[i][0] > dl.max[0]) {
            dl.max[0] = va[i][0];
        }
        /* Get min and max y values. */
        if ( va[i][1] <= dl.min[1]) {
            dl.min[1] = va[i][1];
        } else if ( va[i][1] > dl.max[1]) {
            dl.max[1] = va[i][1];
        }
        /* Get min and max z values. */
        if ( va[i][2] <= dl.min[2]) {
            dl.min[2] = va[i][2];
        } else if ( va[i][2] > dl.max[2]) {
            dl.max[2] = va[i][2];
        }
    }
    return dl;
}
/* Loads the appropriate Material with given name and importand Material infos. */
const void loadMaterial(Material *mtr, const char name[]) {
    FILE *fp;
    fp = fopen("tables/materials.dat", "rb");

    if (!fp){
        fprintf(stderr, "Could not open material files < tables/%s >! loadMaterial() -- fopen().\n", name);
        return;
    } else {
        while (!feof(fp)) {

            fread(mtr, sizeof(Material), 1, fp);

            if ( strncmp(mtr->name, name, strlen(name)) == 0 ) {
                fclose(fp);
                return;
            }
        }
    }
    fclose(fp);
    fprintf(stderr, "Material < %s > not found!\n", name);
}
/* Loads the appropriate Textures and importand Texture infos. */
const void loadtexture(Mesh *m, const unsigned int lvl) {
    if ( !m->material.texlevels )
        return;

    BMP bmp;

    /* Free the previous allocated texture if exists. */
    if ( m->material.texture )
        free(m->material.texture);

    size_t len = 15 + (strlen(m->material.name) * 2) + strlen(m->material.texlvl[lvl]);
    char texpath[len];
    snprintf(texpath, len, "textures/%s/%s%s.bmp", m->material.name, m->material.name, m->material.texlvl[lvl]);

    FILE *fp;
    fp = fopen(texpath, "rb");

    if (!fp){
        fprintf(stderr, "Could not open file < %s >! loadtexture() -- fopen().\n", texpath);
        return;
    } else {
        fread(&bmp.header, sizeof(BMP_Header), 1, fp);
        fseek(fp, 14, SEEK_SET);
        fread(&bmp.info, sizeof(BMP_Info), 1, fp);
        fseek(fp, (14 + bmp.info.Size), SEEK_SET);

        m->material.texture_height = bmp.info.Height;
        m->material.texture_width = bmp.info.Width;
        const int texSize = bmp.info.Height * bmp.info.Width;

        m->material.texture = malloc(texSize * 16);
        if (!m->material.texture)
            fprintf(stderr, "Could not allocate memmory for texture: %s. loadtexture()\n", texpath);

        vec4c temp = { 0, 0, 0, 1 };
        for (int i = 0; i < texSize; i++) {
            fread(&temp, 3, 1, fp);
            m->material.texture[i] = __builtin_convertvector(temp, vec4f) / 255.f;
        }
    }
    fclose(fp);
}
const void adoptdetailMesh(Mesh *m, const int dist) {
    if ( m->lodlevels < 1 )
        return;

    const int lcache_0 = m->currentlod;

    if ( (dist >= 0) && (dist <= 20 * m->scale) ) {
        m->currentlod = 1;
    } else if ( (dist > 20 * m->scale) && (dist <= 40 * m->scale) ) {
        m->currentlod = 2;
    } else if ( (dist > 40 * m->scale) && (dist <= 60 * m->scale) ) {
        m->currentlod = 3;
    } else if ( (dist > 60 * m->scale) && (dist <= 80 * m->scale) ) {
        m->currentlod = 4;
    } else if ( (dist > 80 * m->scale) && (dist <= 100 * m->scale) ) {
        m->currentlod = 5;
    } else if ( (dist > 100 * m->scale) ) {
        m->currentlod = 6;
    }

    if ( m->currentlod > m->lodlevels )
        m->currentlod = m->lodlevels;

    if ( lcache_0 != m->currentlod ) {
        free(m->f);
        loadMesh(m, m->name, m->currentlod);
        printf("adoptDetailMesh()\n");
    }
}
const void adoptdetailTexture(Mesh *m, const int dist) {
    if ( !m->material.texlevels )
        return;

    const int lcache_0 = m->material.texlod;

    if ( (dist >= 0 && dist <= (20 * m->scale) ) && (m->material.texlod != 0) ) {
        m->material.texlod = 0;
    } else if ( (dist > (20 * m->scale)) && (dist <= (40 * m->scale)) ) {
        m->material.texlod = 1;
    } else if ( (dist > (40 * m->scale)) && (dist <= (60 * m->scale)) ) {
        m->material.texlod = 2;
    } else if ( (dist > (60 * m->scale)) && (dist <= (80 * m->scale)) ) {
        m->material.texlod = 3;
    } else if ( (dist > (80 * m->scale)) && (dist <= (100 * m->scale)) ) {
        m->material.texlod = 4;
    } else if ( (dist > (100 * m->scale)) && (dist <= (300 * m->scale)) ) {
        m->material.texlod = 5;
    } else if ( (dist > (300 * m->scale)) && (dist <= (400 * m->scale)) ) {
        m->material.texlod = 6;
    } else if ( (dist > (400 * m->scale)) && (dist <= (500 * m->scale)) ) {
        m->material.texlod = 7;
    } else if ( dist > (500 * m->scale) ) {
        m->material.texlod = 8;
    }

    if ( !m->material.init ) {
        loadtexture(m, m->material.texlod);
        m->material.init = 1;
        printf("Initializing Texture of mesh.id: %d\n", m->id);
        return;
    }

    if ( strcmp(m->material.texlvl[lcache_0], m->material.texlvl[m->material.texlod]) != 0 ) {
        loadtexture(m, m->material.texlod);
        printf("adoptDetailTexture()\n");
    }
}
// const void enWorldMesh(Mesh *m) {
//     Mat4x4 sclMatrix, trMatrix, enWorldMatrix;

//     vec4f pos = { 0 };
//     Mat4x4 mfQ = MatfromQuat(m->Q, pos);
//     sclMatrix = mxm(mfQ, scaleMatrix(m->scale));
//     trMatrix = translationMatrix(m->pivot[0], m->pivot[1], m->pivot[2]);
//     enWorldMatrix = mxm(sclMatrix, trMatrix);

//     m->v = setvecsarrayxm(m->v, m->v_indexes, enWorldMatrix);
//     m->n = setvecsarrayxm(m->n, m->n_indexes, enWorldMatrix);
// }
const void enworldBbox(Mesh *m) {
    Mat4x4 sclMatrix, trMatrix, enWorldMatrix;

    vec4f pos = { 0 };
    Mat4x4 mfQ = MatfromQuat(m->Q, pos);
    sclMatrix = mxm(mfQ, scaleMatrix(m->scale));
    trMatrix = translationMatrix(m->pivot[0], m->pivot[1], m->pivot[2]);
    enWorldMatrix = mxm(sclMatrix, trMatrix);

    m->bbox.v = setvecsarrayxm(m->bbox.v, m->bbox.v_indexes, enWorldMatrix);
}
// const void placeMesh(Mesh *m, const vec4f pos) {
//     Mat4x4 trMatrix = translationMatrix(pos[0], pos[1], pos[2]);

//     m->v = setvecsarrayxm(m->v, m->v_indexes, trMatrix);
//     m->n = setvecsarrayxm(m->n, m->n_indexes, trMatrix);
//     m->pivot = pos;
// }
/* Check and set visibillity of scene objects seen from given meshes pivot point and direction. viewProj: (1 for Prespective and 0 for orthographic Projection).*/
const void checkVisibles(Scene *s, Mesh *m, const int viewProj) {
    vec4f up = { 0.f, -1.f, 0.f };
    vec4f u = cross_product(m->mvdir, up);
    vec4f v = cross_product(u, m->mvdir);

    Mat4x4 collMat;
    Mat4x4 lk = lookat(m->pivot, u, v, m->mvdir);

    if (viewProj)
        collMat = mxm(inverse_mat(lk), perspMat);
    else
        collMat = mxm(inverse_mat(lk), orthoMat);

    // checkVisibility(s->m, s->m_indexes, collMat, viewProj);
}
// /* Ccheck what is visible from a given point of view.Must be implemented with bounding boxes. */
// const static void checkVisibility(Mesh *m, const int len, Mat4x4 vm, const int viewProj) {
//     vec4f *vec_arr;

//     for (int i = 0; i < len; i++) {

//         /* Thats a fix for unitialized meshes that cannot become visible due to no vectors initialization. That will be corrected with bounding boxes. */
//         if (!m[i].v_indexes) {
//             m[i].visible = 1;
//             continue;
//         }

//         vec_arr = vecsarrayxm(m[i].v, m[i].v_indexes, vm);

//         if (!viewProj)
//             for (int j = 0; j < m[i].v_indexes; j++) {
//                 /* We save Clipp space z for frustum culling because Near and far planes are defined in this Space. */
//                 float z = vec_arr[j][2];

//                 if (vec_arr[j][3] > 0) {
//                     vec_arr[j] /= vec_arr[j][3];
//                     vec_arr[j][2] = z;
//                 }
//             }

//         DimensionsLimits dm = getDimensionsLimits(vec_arr, m[i].v_indexes);

//         if ( ((dm.min[2] > FPlane ) || (dm.max[2] < NPlane)) ||
//             ((dm.min[1] > 1.f) || (dm.max[1] < -1.f)) ||
//             ((dm.min[0] > 1.f) || (dm.max[0] < -1.f)) ) {

//             free(vec_arr);
//             m[i].visible = 0;
//             continue;
//         }
//         free(vec_arr);
//         m[i].visible = 1;
//     }
// }
/* Displays given vector on screen given the view Matrix. */
const void displayVec4f(const vec4f v_start, const vec4f v_end, const Mat4x4 vm) {
    vec4f temp_start = vecxm(v_start, vm);
    vec4f temp_end = vecxm(v_end, vm);

    if (temp_start[3] > 0)
        temp_start /= temp_start[3];
    if (temp_end[3] > 0)
        temp_end /= temp_end[3];

    temp_start[0] = ((1 + temp_start[0]) * half_screen[0]) + 0.5;
    temp_start[1] = ((1 + temp_start[1]) * half_screen[1]) + 0.5;

    temp_end[0] = ((1 + temp_end[0]) * half_screen[0]) + 0.5;
    temp_end[1] = ((1 + temp_end[1]) * half_screen[1]) + 0.5;

    XDrawLine(displ, mainwin, gc, temp_start[0], temp_start[1], temp_end[0], temp_end[1]);
}
/* Displays given vector on screen given the view Matrix. */
const void displayBbox(vec4f v[], const int v_indexes, const Mat4x4 vm) {
    /* Thats a fix for unitialized meshes that cannot become visible due to no vectors initialization. That will be corrected with bounding boxes. */
    if (!v_indexes) {
        return;
    }

    // vec4f *vec_arr = malloc(v_indexes * 16);
    vec4f *vec_arr = vecsarrayxm(v, v_indexes, vm);

    for (int j = 0; j < v_indexes; j++) {
        /* We save Clipp space z for frustum culling because Near and far planes are defined in this Space. */
        float z = vec_arr[j][2];

        if (vec_arr[j][3] > 0) {
            vec_arr[j] /= vec_arr[j][3];
            vec_arr[j][2] = z;
        }
    }

    DimensionsLimits dm = getDimensionsLimits(vec_arr, v_indexes);

    dm.min[0] = ((1 + dm.min[0]) * half_screen[0]) + 0.5;
    dm.min[1] = ((1 + dm.min[1]) * half_screen[1]) + 0.5;

    dm.max[0] = ((1 + dm.max[0]) * half_screen[0]) + 0.5;
    dm.max[1] = ((1 + dm.max[1]) * half_screen[1]) + 0.5;

    XDrawRectangle(displ, mainwin, gc, dm.min[0], dm.min[1], dm.max[0] - dm.min[0], dm.max[1] - dm.min[1]);
    free(vec_arr);
    return;
}
/* Initializing Mesh a from Mesh b. */
const void initMesh(Mesh *a, const Mesh *b) {
    *a = *b;
    size_t fsize = sizeof(face) * b->f_indexes;
    size_t bbox_v = sizeof(vec4f) * b->bbox.v_indexes;

    a->f = malloc(fsize);
    memcpy(a->f, b->f, fsize);

    a->bbox.v = malloc(bbox_v);
    memcpy(a->bbox.v, b->bbox.v, bbox_v);
}
/* Releases all members of the given Mesh. */
const void releaseMesh(Mesh *m) {
    free(m->f);
    free(m->bbox.v);
}


