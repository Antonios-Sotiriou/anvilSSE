#include "headers/general_functions.h"

const static void checkVisibility(Mesh *m, const int len, Mat4x4 vm);

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
        .minX = dl.maxX = va[0][0],
        .minY = dl.maxY = va[0][1],
        .minZ = dl.maxZ = va[0][2]
    };

    for (int i = 1; i < len; i++) {
        /* Get min and max x values. */
        if ( va[i][0] <= dl.minX) {
            dl.minX = va[i][0];
        } else if ( va[i][0] > dl.maxX) {
            dl.maxX = va[i][0];
        }
        /* Get min and max y values. */
        if ( va[i][1] <= dl.minY) {
            dl.minY = va[i][1];
        } else if ( va[i][1] > dl.maxY) {
            dl.maxY = va[i][1];
        }
        /* Get min and max z values. */
        if ( va[i][2] <= dl.minZ) {
            dl.minZ = va[i][2];
        } else if ( va[i][2] > dl.maxZ) {
            dl.maxZ = va[i][2];
        }
    }
    return dl;
}
/* Loads the appropriate Material with given name and importand Material infos. */
const void loadmaterial(Material *mtr, const char name[]) {
    FILE *fp;
    fp = fopen("tables/materials.dat", "rb");

    if (!fp){
        fprintf(stderr, "Could not open material files < tables/%s >! loadmaterial() -- fopen().\n", name);
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
    if (!m->material.texlevels)
        return;

    BMP bmp;

    /* Free the previous allocated texture if exists. */
    if (m->material.texture)
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

        /* Subtract 1 from Texture width and height because counting starts from 0; */
        m->material.texture_height = bmp.info.Height;
        m->material.texture_width = bmp.info.Width;
        const int texSize = bmp.info.Height * bmp.info.Width;

        m->material.texture = malloc(texSize * 4);
        if (!m->material.texture)
            fprintf(stderr, "Could not allocate memmory for texture: %s. loadtexture()\n", texpath);

        for (int i = 0; i < texSize; i++) {
            fread(&m->material.texture[i], 3, 1, fp);
            m->material.texture[i][3] = (unsigned char)255;
        }
    }
    fclose(fp);
}
const void adoptdetailMesh(Mesh *m) {
    if ( m->lodlevels < 1 )
        return;

    const int distance = len_vec(m->pivot - lookAt.m[3]);
    const int lcache_0 = m->currentlod;

    if ( (distance >= 0 && distance <= (20 * m->scale) ) && (m->currentlod != 1) ) {
        m->currentlod = 1;
    } else if ( (distance > (20 * m->scale)  && distance <= (40 * m->scale) ) && (m->currentlod != 2) ) {
        m->currentlod = 2;
    } else if ( (distance > (40 * m->scale)  && distance <= (60 * m->scale) ) && (m->currentlod != 3) ) {
        m->currentlod = 3;
    } else if ( (distance > (60 * m->scale)  && distance <= (80 * m->scale) ) && (m->currentlod != 4) ) {
        m->currentlod = 4;
    } else if ( (distance > (80 * m->scale)  && distance <= (100 * m->scale) ) && (m->currentlod != 5) ) {
        m->currentlod = 5;
    } else if ( (distance > (100 * m->scale)) && (m->currentlod != 6) ) {
        m->currentlod = 6;
    }

    if (m->currentlod > m->lodlevels)
        m->currentlod = m->lodlevels;

    if (lcache_0 != m->currentlod) {
        releaseMesh(m);
        loadmesh(m, m->name, m->currentlod);
        reWorldMesh(m);
    }
}
const void adoptdetailTexture(Mesh *m) {
    if ( !m->material.texlevels )
        return;

    const int distance = len_vec(m->pivot - lookAt.m[3]);
    const int lcache_0 = m->material.texlod - 1;

    if ( (distance >= 0 && distance <= (20 * m->scale) ) && (m->material.texlod != 1) ) {
        m->material.texlod = 1;
    } else if ( (distance > (20 * m->scale)  && distance <= (40 * m->scale) ) && (m->material.texlod != 2) ) {
        m->material.texlod = 2;
    } else if ( (distance > (40 * m->scale)  && distance <= (60 * m->scale) ) && (m->material.texlod != 3) ) {
        m->material.texlod = 3;
    } else if ( (distance > (60 * m->scale)  && distance <= (80 * m->scale) ) && (m->material.texlod != 4) ) {
        m->material.texlod = 4;
    } else if ( (distance > (80 * m->scale)  && distance <= (100 * m->scale) ) && (m->material.texlod != 5) ) {
        m->material.texlod = 5;
    } else if ( (distance > (100 * m->scale)  && distance <= (300 * m->scale) ) && (m->material.texlod != 6) ) {
        m->material.texlod = 6;
    } else if ( (distance > (300 * m->scale)  && distance <= (400 * m->scale) ) && (m->material.texlod != 7) ) {
        m->material.texlod = 7;
    } else if ( (distance > (400 * m->scale)  && distance <= (500 * m->scale) ) && (m->material.texlod != 8) ) {
        m->material.texlod = 8;
    } else if ( (distance > (500 * m->scale)) && (m->material.texlod != 9) ) {
        m->material.texlod = 9;
    }

    const int index = m->material.texlod - 1;
    if (strcmp(m->material.texlvl[lcache_0], m->material.texlvl[index]) != 0) {
        loadtexture(m, index);
    }
}
const void reWorldMesh(Mesh *m) {
    Mat4x4 sclMatrix, trMatrix, posMatrix;

    vec4f pos = { 0 };
    Mat4x4 mfQ = MatfromQuat(m->Q, pos);
    sclMatrix = mxm(mfQ, scaleMatrix(m->scale));
    trMatrix = translationMatrix(m->pivot[0], m->pivot[1], m->pivot[2]);
    posMatrix = mxm(sclMatrix, trMatrix);

    m->v = setvecsarrayxm(m->v, m->v_indexes, posMatrix);
    m->n = setvecsarrayxm(m->n, m->n_indexes, posMatrix);
}
const void placeMesh(Mesh *m, const vec4f pos) {
    Mat4x4 trMatrix = translationMatrix(pos[0], pos[1], pos[2]);

    m->v = setvecsarrayxm(m->v, m->v_indexes, trMatrix);
    m->n = setvecsarrayxm(m->n, m->n_indexes, trMatrix);
    m->pivot = pos;
}
const void frustumCulling(Mesh *m, const int len) {
    vec4f *vec_arr;
    DimensionsLimits dm;
    for (int i = 0; i < len; i++) {

        /* Thats a fix for unitialized meshes that cannot become visible due to no vectors initialization. That will be corrected with bounding boxes. */
        if (!m[i].v_indexes) {
            m[i].visible = 1;
            continue;
        }

        vec_arr = vecsarrayxm(m[i].v, m[i].v_indexes, worldMat);

        for (int j = 0; j < m[i].v_indexes; j++) {
            /* We save Clipp space z for frustum culling because Near and far planes are defined in this Space. */
            float z = vec_arr[j][2];

            if (vec_arr[j][3] > 0) {
                vec_arr[j] /= vec_arr[j][3];
                vec_arr[j][2] = z;
            }
        }

        dm = getDimensionsLimits(vec_arr, m[i].v_indexes);

        vec4f min = { dm.minX, dm.minY, dm.minZ, 1.f };
        vec4f max = { dm.maxX, dm.maxY, dm.maxZ, 1.f };


        min[0] = ((1 + min[0]) * HALFW) + 0.5;
        min[1] = ((1 + min[1]) * HALFH) + 0.5;

        max[0] = ((1 + max[0]) * HALFW) + 0.5;
        max[1] = ((1 + max[1]) * HALFH) + 0.5;

        // XDrawRectangle(displ, mainwin, gc, min[0], min[1], max[0] - min[0], max[1] - min[1]);

        if ( ((min[2] > FPlane) || (max[2] < NPlane)) ||
             ((min[1] > 1000) || (max[1] < 0)) ||
             ((min[0] > 1000) || (max[0] < 0)) ) {

            free(vec_arr);
            m[i].visible = 0;
            continue;
        }
        free(vec_arr);
        m[i].visible = 1;
    }
}
const int checkVisibles(Scene *s, Mesh *m) {
    vec4f up = { 0.f, -1.f, 0.f };
    vec4f u = cross_product(m->mvdir, up);
    vec4f v = cross_product(u, m->mvdir);

    Mat4x4 collMat;
    Mat4x4 lk = lookat(m->pivot, u, v, m->mvdir);

    if (!PROJECTIONVIEW)
        collMat = mxm(inverse_mat(lk), perspMat);
    else
        collMat = mxm(inverse_mat(lk), orthoMat);

    checkVisibility(s->m, s->m_indexes, collMat);
}
const static void checkVisibility(Mesh *m, const int len, Mat4x4 vm) {
    vec4f *vec_arr;
    DimensionsLimits dm;

    for (int i = 0; i < len; i++) {

        /* Thats a fix for unitialized meshes that cannot become visible due to no vectors initialization. That will be corrected with bounding boxes. */
        if (!m[i].v_indexes) {
            m[i].visible = 1;
            continue;
        }

        vec_arr = vecsarrayxm(m[i].v, m[i].v_indexes, vm);

        if (!PROJECTIONVIEW)
            for (int j = 0; j < m[i].v_indexes; j++) {
                /* We save Clipp space z for frustum culling because Near and far planes are defined in this Space. */
                float z = vec_arr[j][2];

                if (vec_arr[j][3] > 0) {
                    vec_arr[j] /= vec_arr[j][3];
                    vec_arr[j][2] = z;
                }
            }

        dm = getDimensionsLimits(vec_arr, m[i].v_indexes);

        if ( ((dm.minZ > FPlane ) || (dm.maxZ < NPlane)) ||
            ((dm.minY > 1) || (dm.maxY < -1)) ||
            ((dm.minX > 1) || (dm.maxX < -1)) ) {

            free(vec_arr);
            m[i].visible = 0;
            continue;
        }
        free(vec_arr);
        m[i].visible = 1;
    }
}
/* Initializing Mesh a from Mesh b. */
const void initMesh(Mesh *a, const Mesh *b) {
    *a = *b;
    size_t vsize = sizeof(vec4f) * b->v_indexes;
    size_t nsize = sizeof(vec4f) * b->n_indexes;
    size_t tsize = sizeof(vec2f) * b->t_indexes;
    size_t fsize = sizeof(unsigned int) * b->f_indexes;

    a->v = malloc(vsize);
    memcpy(a->v, b->v, vsize);
    a->v_indexes = b->v_indexes;

    a->n = malloc(nsize);
    memcpy(a->n, b->n, nsize);
    a->n_indexes = b->n_indexes;

    a->t = malloc(tsize);
    memcpy(a->t, b->t, tsize);
    a->t_indexes = b->t_indexes;

    a->f = malloc(fsize);
    memcpy(a->f, b->f, fsize);
    a->f_indexes = b->f_indexes;
}
/* Releases all members of the given Mesh. */
const void releaseMesh(Mesh *m) {
    free(m->v);
    free(m->n);
    free(m->t);
    free(m->f);
}


