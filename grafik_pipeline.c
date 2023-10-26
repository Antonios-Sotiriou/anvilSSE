#include "headers/grafik_pipeline.h"

#include "headers/frustum_map.h"
#include "headers/logging.h"

const static void adoptdetail(Mesh *m);
const static void assemblyfaces(Mesh *m, const int len);
const static void ppdiv(Mesh *m, const int len);
const static Mesh bfculling(const Mesh m, const int len);
const static int viewtoscreen(Mesh *m, const int len);
const static void rasterize(const Mesh m);

extern int HALFH, HALFW, DEBUG;
extern float FPlane, NPlane;
extern XWindowAttributes wa;
extern Mat4x4 viewMat, worldMat, reperspMat, lookAt, lightMat, perspMat;


// extern vec4f viewFrustum[8];
vec4f viewFr[8] = {
    { -0.5f, -0.5f, 0.001f, 1.f },
    { 0.5f, -0.5f, 0.001f, 1.f },
    { 0.5f, 0.5f, 0.001f, 1.f },
    { -0.5f, 0.5f, 0.001f, 1.f },
    { -0.5f, -0.5f, 1.f, 1.f },
    { 0.5f, -0.5f, 1.f, 1.f },
    { 0.5f, 0.5f, 1.f, 1.f },
    { -0.5f, 0.5f, 1.f, 1.f },
};

void vfcorners(void) {
    float aspectRatio = (float)wa.width / (float)wa.height;
    float fovRadius = 1.f / tanf(45.f * 0.5f / 180.0f * 3.14159f);

    vec4f nearcenter = (lookAt.m[3] + lookAt.m[2]) * NPlane;
    vec4f farcenter = (lookAt.m[3] + lookAt.m[2]) * FPlane;

    float nearHeight = 2.f * tan(fovRadius / 2.f) * NPlane;
    float farHeight = 2.f * tan(fovRadius / 2.f) * FPlane;
    float nearWidth = nearHeight * aspectRatio;
    float farWidth = farHeight * aspectRatio;
    // printf("nearcenter: ");
    // logVec4f(nearcenter);
    // printf("farcenter: ");
    // logVec4f(farcenter);
    // printf("nearHeight: %f,    nearWidth: %f,    farHeight: %f,    farWidth: %f\n", nearHeight, nearWidth, farHeight, farWidth);

    viewFr[2] = nearcenter + (lookAt.m[1] * (nearHeight * 0.5f)) + (lookAt.m[0] * (nearWidth * 0.5f));
    viewFr[3] = nearcenter - (lookAt.m[1] * (nearHeight * 0.5f)) + (lookAt.m[0] * (nearWidth * 0.5f));
    viewFr[6] = nearcenter + (lookAt.m[1] * (nearHeight * 0.5f)) - (lookAt.m[0] * (nearWidth * 0.5f));
    viewFr[7] = nearcenter - (lookAt.m[1] * (nearHeight * 0.5f)) - (lookAt.m[0] * (nearWidth * 0.5f));

    viewFr[0] = farcenter + (lookAt.m[1] * (farHeight * 0.5f)) + (lookAt.m[0] * (farWidth * 0.5f));
    viewFr[1] = farcenter - (lookAt.m[1] * (farHeight * 0.5f)) + (lookAt.m[0] * (farWidth * 0.5f));
    viewFr[4] = farcenter + (lookAt.m[1] * (farHeight * 0.5f)) - (lookAt.m[0] * (farWidth * 0.5f));
    viewFr[5] = farcenter - (lookAt.m[1] * (farHeight * 0.5f)) - (lookAt.m[0] * (farWidth * 0.5f));
}
extern Display *displ;
extern Window mainwin;
extern GC gc;
// const void frustum_transform(vec4f vf[]) {
//     // vfcorners();
//     vec4f a[8], b[8], c[8];
//     for (int i = 0; i < 8; i++) {
//         // vf[i] /= vf[i][3];
//         // a[i] = vecxm(vf[i], inverse_mat(worldMat));
//         // a[i] /= a[i][3];
//         // // b[i] = vecxm(a[i], lookAt);
//         // Mat4x4 sc = scaleMatrix(0.1f);
//         // c[i] = vecxm(vf[i], sc);
//         c[i] = vecxm(vf[i], worldMat);

//         // c[i] /= c[i][3];
//         // // c[i] = vf[i];
//         // c[i][0] = ((1 + c[i][0]) * HALFW) + 0.5;
//         // c[i][1] = ((1 + c[i][1]) * HALFH) + 0.5;
//         // c[i][2] = 1.f / c[i][2];
//         logVec4f(c[i]);
//     }

//     // XGCValues gcv;
//     // gcv.foreground = 0xcccccc;
//     // XChangeGC(displ, gc, GCForeground, &gcv);
//     // XDrawLine(displ, mainwin, gc, c[0][0], c[0][1], c[1][0], c[1][1]);
//     // XDrawLine(displ, mainwin, gc, c[1][0], c[1][1], c[2][0], c[2][1]);
//     // XDrawLine(displ, mainwin, gc, c[2][0], c[2][1], c[3][0], c[3][1]);
//     // XDrawLine(displ, mainwin, gc, c[3][0], c[3][1], c[0][0], c[0][1]);
//     // XDrawLine(displ, mainwin, gc, c[4][0], c[4][1], c[5][0], c[5][1]);
//     // XDrawLine(displ, mainwin, gc, c[5][0], c[5][1], c[6][0], c[6][1]);
//     // XDrawLine(displ, mainwin, gc, c[6][0], c[6][1], c[7][0], c[7][1]);
//     // XDrawLine(displ, mainwin, gc, c[7][0], c[7][1], c[4][0], c[4][1]);
//     exit(0);
// }

/* Passes the scene Meshes throught the graphic pipeline. */
const void grafikPipeline(Scene s) {
    Mesh cache = { 0 };
    vfcorners();
    // frustum_transform(viewFr);
    for (int i = 0; i < s.m_indexes; i++) {
        adoptdetail(&s.m[i]);

        initMesh(&cache, s.m[i]);

        // if (i > 0) {
            cache.v = vecsarrayxm(s.m[i].v, s.m[i].v_indexes, worldMat);
            cache.n = vecsarrayxm(s.m[i].n, s.m[i].n_indexes, viewMat);
        // } else {
        //     cache.v = malloc(16 * s.m[i].v_indexes);
        //     memcpy(cache.v, viewFr, 16 * s.m[i].v_indexes);
        //     // Mat4x4 sc = scaleMatrix(0.1f);
        //     // Mat4x4 tr = translationMatrix(0.f, 0.f, 100.f);
        //     // Mat4x4 nm = mxm(sc, tr);
        //     // cache.v = setvecsarrayxm(cache.v, cache.v_indexes, sc);
        //     // rotate_x(&cache, 1);
        //     cache.v = setvecsarrayxm(cache.v, cache.v_indexes, worldMat);
        //     cache.n = vecsarrayxm(s.m[i].n, s.m[i].n_indexes, viewMat);
        // }

        /* Assembly and create the faces from the mesh vertices, normals and texture arrays, through the indexes. */
        assemblyfaces(&cache, cache.f_indexes);

        /* Clipping against near Plane in View Space. */
        vec4f plane_near_p = { 0.0f, 0.0f, NPlane },
                plane_near_n = { 0.0f, 0.0f, 1.0f };
        cache = clipp(cache, plane_near_p, plane_near_n);
        if (!cache.f_indexes) {
            releaseMesh(&cache);
            continue;
        }

        /* Clipping against far Plane in View Space. */
        vec4f plane_far_p = { 0.0f, 0.0f,  FPlane},
                plane_far_n = { 0.0f, 0.0f, -1.0f };
        cache = clipp(cache, plane_far_p, plane_far_n);
        if (!cache.f_indexes) {
            releaseMesh(&cache);
            continue;
        }

        /* Applying perspective division. */
        ppdiv(&cache, cache.f_indexes);

        /* Applying Backface culling before we proceed to Screen Space transformation and view Port clipping. */
        if (cache.cull)
            cache = bfculling(cache, cache.f_indexes);
        if (!cache.f_indexes) {
            releaseMesh(&cache);
            continue;
        }

        /* Sending to translation from NDC to Screen Coordinates. */
        if (!viewtoscreen(&cache, cache.f_indexes)) {
            releaseMesh(&cache);
            continue;
        }
        // mapPipeline(cache);
        rasterize(cache);

        releaseMesh(&cache);
    }
}
const static void adoptdetail(Mesh *m) {
    const int distance = len_vec(m->pivot - viewMat.m[3]);
    // printf("length 1: %d\n", distance);
    const int lcache = m->lvlofdetail;

    if ( (distance > 0 && distance <= 200) && (m->lvlofdetail != 0) ) {
        printf("lvl of detail 1\n");
        m->lvlofdetail = 0;
    } else if ( (distance > 200 && distance <= 400) && (m->lvlofdetail != 1) ) {
        printf("lvl of detail 2\n");
        m->lvlofdetail = 1;
    } else if ( (distance > 400 && distance <= 600) && (m->lvlofdetail != 2) ) {
        printf("lvl of detail 3\n");
        m->lvlofdetail = 2;
    } else if ( (distance > 600 && distance <= 800) && (m->lvlofdetail != 3) ) {
        printf("lvl of detail 4\n");
        m->lvlofdetail = 3;
    } else if ( (distance > 800 && distance <= 1000) && (m->lvlofdetail != 4) ) {
        printf("lvl of detail 5\n");
        m->lvlofdetail = 4;
    } else if ( (distance > 1000 && distance <= 3000) && (m->lvlofdetail != 5) ) {
        printf("lvl of detail 6\n");
        m->lvlofdetail = 5;
    } else if ( (distance > 3000 && distance <= 4000) && (m->lvlofdetail != 6) ) {
        printf("lvl of detail 7\n");
        m->lvlofdetail = 6;
    } else if ( (distance > 4000 && distance <= 5000) && (m->lvlofdetail != 7) ) {
        printf("lvl of detail 8\n");
        m->lvlofdetail = 7;
    } else if ( (distance > 5000 && distance < FPlane) && (m->lvlofdetail != 8) ) {
        printf("lvl of detail 9\n");
        m->lvlofdetail = 8;
    }

    if (!m->material.tex_levels)
        return;

    if (strcmp(m->material.texlvl[lcache], m->material.texlvl[m->lvlofdetail]) != 0) {
        loadtexture(m);
    }
}
/* Assosiates vertices coordinate values from vector array through indexes. */
const static void assemblyfaces(Mesh *m, const int len) {
    for (int i = 0; i < len; i++) {
        m->f[i].v[0] = m->v[m->f[i].a[0]];
        m->f[i].v[1] = m->v[m->f[i].b[0]];
        m->f[i].v[2] = m->v[m->f[i].c[0]];

        m->f[i].vt[0] = m->t[m->f[i].a[1]];
        m->f[i].vt[1] = m->t[m->f[i].b[1]];
        m->f[i].vt[2] = m->t[m->f[i].c[1]];

        m->f[i].vn[0] = m->n[m->f[i].a[2]];
        m->f[i].vn[1] = m->n[m->f[i].b[2]];
        m->f[i].vn[2] = m->n[m->f[i].c[2]];
    }
}
/* Perspective division. */
const static void ppdiv(Mesh *m, const int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 3; j++) {
            float w = m->f[i].v[j][3];
            m->f[i].v[j] /= w;
            m->f[i].v[j][3] = w;
        }
    }
}
/* Backface culling.Discarding Triangles that should not be painted.Creating a new dynamic Mesh stucture Triangles array. */
const static Mesh bfculling(const Mesh m, const int len) {
    Mesh r = m;
    size_t size = sizeof(face);
    int counter = 1;
    int index = 0;
    r.f = malloc(size);
    if (!r.f)
        fprintf(stderr, "Could not allocate memory - bfculling() - malloc\n");

    for (int i = 0; i < len; i++) {
        if (winding(m.f[i]) > 0.00) {
            r.f = realloc(r.f, size * counter);

            if (!r.f)
                fprintf(stderr, "Could not allocate memory - bfculling() - realloc\n");

            r.f[index] = m.f[i];

            counter++;
            index++;
        }
    }
    r.f_indexes = index;
    free(m.f);
    return r;
}
/* Translates the Mesh's Triangles from world to Screen Coordinates. */
const static int viewtoscreen(Mesh *m, const int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 3; j++) {

            m->f[i].vt[j] /= m->f[i].v[j][3];

            m->f[i].v[j][0] = ((1 + m->f[i].v[j][0]) * HALFW) + 0.5; /* adding 0.5 at this point so we can convert to integer at drawing stage. */
            m->f[i].v[j][1] = ((1 + m->f[i].v[j][1]) * HALFH) + 0.5; /* adding 0.5 at this point so we can convert to integer at drawing stage. */
            m->f[i].v[j][2] = 1.f / m->f[i].v[j][2];
            m->f[i].v[j][3] = 1.f / m->f[i].v[j][3];
        }
    }

    /* Viewport clipping. */
    vec4f plane_up_p = { 0.0f, 0.0f, 0.0f },
        plane_up_n = { 0.0f, 1.0f, 0.0f };
    *m = clipp(*m, plane_up_p, plane_up_n);
    if(!m->f_indexes)
        return 0;

    vec4f plane_down_p = { 0.0f, wa.height - 1.0f, 0.0f },
        plane_down_n = { 0.0f, -1.0f, 0.0f };
    *m = clipp(*m, plane_down_p, plane_down_n);
    if(!m->f_indexes)
        return 0;

    vec4f plane_left_p = { 0.0f, 0.0f, 0.0f },
        plane_left_n = { 1.0f, 0.0f, 0.0f };
    *m = clipp(*m, plane_left_p, plane_left_n);
    if(!m->f_indexes)
        return 0;

    vec4f plane_right_p = { wa.width - 1.0f, 0.0f, 0.0f },
        plane_right_n = { -1.0f, 0.0f, 0.0f };
    *m = clipp(*m, plane_right_p, plane_right_n);
    if(!m->f_indexes)
        return 0;

    return 1;
}
/* Rasterize given Mesh by passing them to the appropriate function. */
const static void rasterize(const Mesh m) {
    if (DEBUG == 1) {
        edgeMesh(m, m.material.basecolor);
    } else if (DEBUG == 2) {
        fillMesh(m);
    } else {
        texMesh(m);
    }
}


