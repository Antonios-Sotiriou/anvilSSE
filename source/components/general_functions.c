#include "../../headers/components/general_functions.h"

const static void checkVisibility(Mesh *m, const int len, Mat4x4 vm, const int viewProj);
const static vec4f add_one = { 1.f, 1.f, 0.f, 0.f };
const static vec4f add_half = { 0.5f, 0.5f, 0.f, 0.f };

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
/* Finds the minX, maxX, minY, maxY, minZ maxZ values of given vectors array. AKA (bounding box). */
const  MeshOria getMeshOria(vec4f va[], const int len) {
    MeshOria mo = {
        .minX = mo.minY = mo.minZ = va[0],
        .maxX = mo.maxY = mo.maxZ = va[0],
    };

    for (int i = 1; i < len; i++) {

        if ( va[i][0] <= mo.minX[0]) {
            mo.minX = va[i];
        } else if ( va[i][0] > mo.maxX[0]) {
            mo.maxX = va[i];
        }
        /* Get min and max y values. */
        if ( va[i][1] <= mo.minY[1]) {
            mo.minY = va[i];
        } else if ( va[i][1] > mo.maxY[1]) {
            mo.maxY = va[i];
        }
        /* Get min and max z values. */
        if ( va[i][2] <= mo.minZ[2]) {
            mo.minZ = va[i];
        } else if ( va[i][2] > mo.maxZ[2]) {
            mo.maxZ = va[i];
        }
    }
    return mo;
}
void roundMesh(Mesh *m) {
    for (int i = 0; i < m->bbox.f_indexes; i++)
        for (int x = 0; x < 3; x++)
            m->bbox.f[i].v[x] = round_vec4f(m->bbox.f[i].v[x]);
}
/* Loads the appropriate Material with given name and importand Material infos. */
const void loadMaterial(Material *mtr, const char name[]) {
    FILE *fp;
    fp = fopen("tables/materials.dat", "rb");

    if ( !fp ) {
        fprintf(stderr, "Could not open material files < tables/%s >! loadMaterial() -- fopen().\n", name);
        return;
    } else {
        while ( !feof(fp) ) {

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

    if ( !fp ) {
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

        m->material.texture = malloc(texSize * 4);
        if (!m->material.texture)
            fprintf(stderr, "Could not allocate memmory for texture: %s. loadtexture()\n", texpath);

        vec4c temp = { 0, 0, 0, 255 };
        for (int i = 0; i < texSize; i++) {
            fread(&temp, 3, 1, fp);
            m->material.texture[i] = temp;
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
    return; /* Must be removed for enabled texture level of details. ################################################################## */
    if ( strcmp(m->material.texlvl[lcache_0], m->material.texlvl[m->material.texlod]) != 0 ) {
        loadtexture(m, m->material.texlod);
        printf("adoptDetailTexture()\n");
    }
}
const void enworldBbox(Mesh *m) {
    Mat4x4 sclMatrix, trMatrix, enWorldMatrix;

    vec4f pos = { 0 };
    Mat4x4 mfQ = MatfromQuat(m->Q, pos);
    sclMatrix = mxm(mfQ, scaleMatrix(m->scale));
    trMatrix = translationMatrix(m->cd.v[P][0], m->cd.v[P][1], m->cd.v[P][2]);
    enWorldMatrix = mxm(sclMatrix, trMatrix);

    setvecsarrayxm(m->bbox.v, m->bbox.v_indexes, enWorldMatrix);
    setfacesarrayxm(m->bbox.f, m->bbox.f_indexes, enWorldMatrix);
}
/* Check and set visibillity of scene objects seen from given meshes cd.v[P] point and direction. viewProj: (1 for Prespective and 0 for orthographic Projection).*/
const void checkVisibles(Scene *s, Mesh *m, const int viewProj) {
    vec4f up = { 0.f, -1.f, 0.f };
    vec4f u = cross_product(m->mvdir, up);
    vec4f v = cross_product(u, m->mvdir);

    Mat4x4 collMat;
    Mat4x4 lk = lookat(m->cd.v[P], u, v, m->mvdir);

    if (viewProj)
        collMat = mxm(inverse_mat(lk), perspMatrix);
    else
        collMat = mxm(inverse_mat(lk), orthoMatrix);

    // checkVisibility(s->m, s->m_indexes, collMat, viewProj);
}
// /* Check what is visible from a given point of view.Must be implemented with bounding boxes. */
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
/* Displays given point on screen given the view Matrix. */
const void displayPoint(const vec4f p, const Mat4x4 vm, const int color) {
    Mat4x4 tm = mxm(viewMatrix, perspectiveMatrix(45.f, -1.f, 10.f, __INT32_MAX__));
    vec4f point = vecxm(p, tm);

    if (point[3] > 0)
        point /= point[3];

    point[0] = (1 + point[0]) * half_screen[0];
    point[1] = (1 + point[1]) * half_screen[1];

    GC g;
    XGCValues gv;
    gv.background = 0x000000;
    gv.foreground = color;
    gv.graphics_exposures = False;
    g = XCreateGC(displ, mainwin, GCBackground | GCForeground | GCGraphicsExposures, &gv);

    XFillArc(displ, mainwin, g, point[0] - 2, point[1] - 2, 4, 4, 0, 360 * 64);
    XFreeGC(displ, g);
}
/* Displays given Meshe's pivot point and moving direction on screen given the view Matrix. */
const void displayMeshKinetics(Mesh *m, const Mat4x4 vm) {
    Mat4x4 tm = mxm(viewMatrix, perspectiveMatrix(45.f, -1.f, 10.f, __INT32_MAX__));
    vec4f temp_start = m->cd.v[0];
    vec4f temp_end = m->cd.v[0] + (m->velocity * m->scale);

    setvecxm(&temp_start, tm);
    setvecxm(&temp_end, tm);

    if (temp_start[3] > 0)
        temp_start /= temp_start[3];
    if (temp_end[3] > 0)
        temp_end /= temp_end[3];

    temp_start[0] = (1 + temp_start[0]) * half_screen[0];
    temp_start[1] = (1 + temp_start[1]) * half_screen[1];

    temp_end[0] = (1 + temp_end[0]) * half_screen[0];
    temp_end[1] = (1 + temp_end[1]) * half_screen[1];

    XFillArc(displ, mainwin, gc, temp_start[0] - 2, temp_start[1] - 2, 4, 4, 0, 360 * 64);
    XDrawLine(displ, mainwin, gc, temp_start[0], temp_start[1], temp_end[0], temp_end[1]);
}
/* Displays given Meshe's pivot point and moving direction on screen given the view Matrix. */
const void drawLine(vec4f start, vec4f end, const Mat4x4 vm) {
    Mat4x4 tm = mxm(viewMatrix, perspectiveMatrix(45.f, -1.f, 10.f, __INT32_MAX__));
    vec4f temp_start = start;
    vec4f temp_end = end;

    setvecxm(&temp_start, tm);
    setvecxm(&temp_end, tm);

    if (temp_start[3] > 0)
        temp_start /= temp_start[3];
    if (temp_end[3] > 0)
        temp_end /= temp_end[3];

    temp_start[0] = (1 + temp_start[0]) * half_screen[0];
    temp_start[1] = (1 + temp_start[1]) * half_screen[1];

    temp_end[0] = (1 + temp_end[0]) * half_screen[0];
    temp_end[1] = (1 + temp_end[1]) * half_screen[1];

    XFillArc(displ, mainwin, gc, temp_start[0] - 2, temp_start[1] - 2, 4, 4, 0, 360 * 64);
    XDrawLine(displ, mainwin, gc, temp_start[0], temp_start[1], temp_end[0], temp_end[1]);
}
/* Displays given face on screen given the view Matrix. */
const void displayFace(face *f, const Mat4x4 vm) {
    Mat4x4 tm = mxm(viewMatrix, perspectiveMatrix(45.f, -1.f, 10.f, __INT32_MAX__));
    face r = facexm(*f, tm);

    for (int j = 0; j < 3; j++) {
        /* We save Clipp space z for frustum culling because Near and far planes are defined in this Space. */
        float z = r.v[j][3];

        if (r.v[j][3] > 0) {
            r.v[j] /= r.v[j][3];
            r.v[j][2] = z;
        }

        r.v[j] = (1.f + r.v[j]) * __builtin_convertvector(half_screen, vec4f);
    }

    XDrawLine(displ, mainwin, gc, r.v[0][0], r.v[0][1], r.v[1][0], r.v[1][1]);
    XDrawLine(displ, mainwin, gc, r.v[1][0], r.v[1][1], r.v[2][0], r.v[2][1]);
    XDrawLine(displ, mainwin, gc, r.v[2][0], r.v[2][1], r.v[0][0], r.v[0][1]);
}
/* Displays given face on screen given the view Matrix. */
const void displayFilledFace(face *f, const Mat4x4 vm) {
    Mat4x4 tm = mxm(viewMatrix, perspectiveMatrix(45.f, -1.f, 10.f, __INT32_MAX__));
    face r = facexm(*f, tm);

    for (int j = 0; j < 3; j++) {
        /* We save Clipp space z for frustum culling because Near and far planes are defined in this Space. */
        float z = r.v[j][3];

        if (r.v[j][3] > 0) {
            r.v[j] /= r.v[j][3];
            r.v[j][2] = z;
        }

        r.v[j] = (1.f + r.v[j]) * __builtin_convertvector(half_screen, vec4f);
    }

    XPoint v[3] = {
        {r.v[0][0], r.v[0][1]},
        {r.v[1][0], r.v[1][1]},
        {r.v[2][0], r.v[2][1]}
    };
    XFillPolygon(displ, mainwin, gc, v, 3, Convex, CoordModeOrigin);
}
/* Displays given Bounding Box on screen given the view Matrix. Bounding box must be in World Space. */
const void displayBbox(Mesh *m, const Mat4x4 vm) {
    Mat4x4 tm = mxm(viewMatrix, perspectiveMatrix(45.f, -1.f, 10.f, __INT32_MAX__));
    vec4f *vec_arr = vecsarrayxm(m->bbox.v, m->bbox.v_indexes, tm);

    for (int j = 0; j < m->bbox.v_indexes; j++) {
        /* We save Clipp space z for frustum culling because Near and far planes are defined in this Space. */
        float z = vec_arr[j][2];

        if (vec_arr[j][3] > 0) {
            vec_arr[j] /= vec_arr[j][3];
            vec_arr[j][2] = z;
        }
    }

    DimensionsLimits dm = getDimensionsLimits(vec_arr, m->bbox.v_indexes);

    dm.min[0] = ((1 + dm.min[0]) * half_screen[0]);
    dm.min[1] = ((1 + dm.min[1]) * half_screen[1]);

    dm.max[0] = ((1 + dm.max[0]) * half_screen[0]);
    dm.max[1] = ((1 + dm.max[1]) * half_screen[1]);

    XDrawRectangle(displ, mainwin, gc, dm.min[0], dm.min[1], dm.max[0] - dm.min[0], dm.max[1] - dm.min[1]);
    free(vec_arr);
}
/* Displays given Bounding Box on screen given the view Matrix. Bounding box must be in World Space. */
const void displayBboxFaces(Mesh *m, const Mat4x4 vm) {
    Mat4x4 tm = mxm(viewMatrix, perspectiveMatrix(45.f, -1.f, 10.f, __INT32_MAX__));
    face *vec_arr = facesarrayxm(m->bbox.f, m->bbox.f_indexes, tm);

    for (int i = 0; i < m->bbox.f_indexes; i++) {
        for (int j = 0; j < 3; j++) {
            /* We save Clipp space z for frustum culling because Near and far planes are defined in this Space. */
            float z = vec_arr[i].v[j][2];

            if (vec_arr[i].v[j][3] > 0) {
                vec_arr[i].v[j] /= vec_arr[i].v[j][3];
                vec_arr[i].v[j][2] = z;
            }

            vec_arr[i].v[j] = ((add_one + vec_arr[i].v[j]) * __builtin_convertvector(half_screen, vec4f)) + add_half;
        }

        XDrawLine(displ, mainwin, gc, vec_arr[i].v[0][0], vec_arr[i].v[0][1], vec_arr[i].v[1][0], vec_arr[i].v[1][1]);
        XDrawLine(displ, mainwin, gc, vec_arr[i].v[1][0], vec_arr[i].v[1][1], vec_arr[i].v[2][0], vec_arr[i].v[2][1]);
        XDrawLine(displ, mainwin, gc, vec_arr[i].v[2][0], vec_arr[i].v[2][1], vec_arr[i].v[0][0], vec_arr[i].v[0][1]);
    }

    free(vec_arr);
}
/* Initializing Mesh a from Mesh b. */
const void initMesh(Mesh *a, const Mesh *b) {
    *a = *b;
    size_t fsize = sizeof(face) * b->f_indexes;
    size_t vba_size = sizeof(vec8f) * b->vba_indexes;
    size_t bbox_v = sizeof(vec4f) * b->bbox.v_indexes;
    size_t bbox_f = sizeof(face) * b->bbox.f_indexes;

    a->f = malloc(fsize);
    memcpy(a->f, b->f, fsize);

    a->vba = malloc(vba_size);
    memcpy(a->vba, b->vba, vba_size);

    a->bbox.v = malloc(bbox_v);
    memcpy(a->bbox.v, b->bbox.v, bbox_v);

    a->bbox.f = malloc(bbox_f);
    memcpy(a->bbox.f, b->bbox.f, bbox_f);
}
/* Releases all members of the given Mesh. */
const void releaseMesh(Mesh *m) {
    free(m->f);
    free(m->vba);
    free(m->bbox.v);
    free(m->bbox.f);
}


