#include "headers/frustum_map.h"

#include "headers/world_objects.h"
#include "headers/draw_functions.h"
#include "headers/clipping.h"
#include "headers/logging.h"

extern u_int8_t map_buffer;
extern int HALFW, HALFH, DEBUG, PROJECTIONVIEW;
extern Mat4x4 lookAt, reperspMat, perspMat, orthoMat;
Mat4x4 mapLook, mapView, mapWorld;
extern float FPlane, NPlane;
extern XWindowAttributes wa;
extern vec4f camera[4];

const static void screentondc(Mesh *m, const int len);
const static void maprasterize(const Mesh m);

/* Camera and Global light Source. */
vec4f mapcam[4] = {
    { 0.0f, 10000.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f, 0.0f }
};

const void mapPipeline(const Mesh m) {
    Mesh cache = { 0 };
    initMesh(&cache, m);
    cache.v = malloc(1);
    // cache.n = malloc(1);
    cache.n = malloc(m.n_indexes * sizeof(vec4f));
    cache.n = memcpy(cache.n, m.n, m.n_indexes * sizeof(vec4f));

    screentondc(&cache, cache.f_indexes);
    maprasterize(cache);
    releaseMesh(&cache);
}
const static void screentondc(Mesh *m, const int len) {
    mapcam[0][0] = camera[0][0];
    mapcam[0][2] = camera[0][2];
    mapLook = lookat(mapcam[0], mapcam[1], mapcam[2], mapcam[3]);
    mapView = inverse_mat(mapLook);

    if (!PROJECTIONVIEW)
        mapWorld = mxm(mapView, perspMat);
    else
        mapWorld = mxm(mapView, orthoMat);

    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 3; j++) {
            m->f[i].v[j][3] = 1.0f / m->f[i].v[j][3];
            m->f[i].vt[j] *= m->f[i].v[j][3];

            m->f[i].v[j][0] = ((m->f[i].v[j][0] / HALFW) - 1.0) * m->f[i].v[j][3];
            m->f[i].v[j][1] = ((m->f[i].v[j][1] / HALFH) - 1.0) * m->f[i].v[j][3];
            m->f[i].v[j][2] = (m->f[i].v[j][2] / 0.5) * m->f[i].v[j][3];

            m->f[i].v[j] = vecxm(m->f[i].v[j], reperspMat);
            m->f[i].v[j][3] = 1.0f;

            m->f[i].v[j] = vecxm(m->f[i].v[j], lookAt);

            /* view space */
            m->f[i].v[j] = vecxm(m->f[i].v[j], mapWorld);
        }
    }

    /* At this Point triangles must be clipped against near plane. */
    vec4f plane_near_p = { 0.0f, 0.0f, NPlane },
          plane_near_n = { 0.0f, 0.0f, 1.0f };
    *m = clipp(*m, plane_near_p, plane_near_n);

    for (int i = 0; i < m->f_indexes; i++) {
        for (int j = 0; j < 3; j++) {
            /* ppdiv */
            float w = m->f[i].v[j][3];
            m->f[i].v[j] /= w;
            m->f[i].v[j][3] = w;

            /* screen space */
            m->f[i].vt[j] /= m->f[i].v[j][3];

            m->f[i].v[j][0] = ((1.0 + m->f[i].v[j][0]) * HALFW) + 0.5;
            m->f[i].v[j][1] = ((1.0 + m->f[i].v[j][1]) * HALFH) + 0.5;
            m->f[i].v[j][2] *= 0.5;
            m->f[i].v[j][3] = 1.0f / m->f[i].v[j][3];
        }
    }
    /* Far Plane clipping and side clipping. */
    vec4f plane_far_p = { 0.0, 0.0,  FPlane},
          plane_far_n = { 0.0, 0.0, 1.0 };
    *m = clipp(*m, plane_far_p, plane_far_n);
    if (!m->f_indexes)
        return;

    vec4f plane_up_p = { 0.0, 0.0, 0.0 },
          plane_up_n = { 0.0, 1.0, 0.0 };
    *m = clipp(*m, plane_up_p, plane_up_n);
    if (!m->f_indexes)
        return;

    vec4f plane_down_p = { 0.0, wa.height - 1.0, 0.0 },
          plane_down_n = { 0.0, -1.0, 0.0 };
    *m = clipp(*m, plane_down_p, plane_down_n);
    if (!m->f_indexes)
        return;

    vec4f plane_left_p = { 0.0, 0.0, 0.0 },
          plane_left_n = { 1.0, 0.0, 0.0 };
    *m = clipp(*m, plane_left_p, plane_left_n);
    if (!m->f_indexes)
        return;

    vec4f plane_right_p = { wa.width - 1.0, 0.0, 0.0 },
          plane_right_n = { -1.0, 0.0, 0.0 };
    *m = clipp(*m, plane_right_p, plane_right_n);
    if (!m->f_indexes)
        return;
}
/* Rasterize given Mesh by passing them to the appropriate function. */
const static void maprasterize(const Mesh m) {
    if (DEBUG == 1) {
        edgeMesh(m, m.material.basecolor);
    } else if (DEBUG == 2) {
        fillMesh(m);
    } else {
        texMesh(m);
    }
}


