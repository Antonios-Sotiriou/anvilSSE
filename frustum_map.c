#include "headers/frustum_map.h"

const static int screentondc(Mesh *m, const int len);
const static void maprasterize(const Mesh m);

Mat4x4 mapLook, mapView, mapWorld;

/* Camera and Global light Source. */
vec4f mapcam[4] = {
    { 0.0f, 1000.0f, 0.0f, 1.0f },
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f, 0.0f }
};

const void mapPipeline(const Scene *s) {

    mapcam[0][0] = eye[0][0];
    mapcam[0][2] = eye[0][2];
    mapLook = lookat(mapcam[0], mapcam[1], mapcam[2], mapcam[3]);
    mapView = inverse_mat(mapLook);

    if (!PROJECTIONVIEW)
        mapWorld = mxm(mapView, perspMat);
    else
        mapWorld = mxm(mapView, orthoMat);

    for (int i = 0; i < s->m_indexes; i++) {

        if (screentondc(&s->m[i], s->m[i].f_indexes))
            maprasterize(s->m[i]);
    }
}
const static int screentondc(Mesh *m, const int len) {

    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 3; j++) {
            m->f[i].v[j][3] = 1.0f / m->f[i].v[j][3];
            m->f[i].vt[j] *= m->f[i].v[j][3];

            m->f[i].v[j][0] = ((m->f[i].v[j][0] / HALFW) - 1.0) * m->f[i].v[j][3];
            m->f[i].v[j][1] = ((m->f[i].v[j][1] / HALFH) - 1.0) * m->f[i].v[j][3];
            m->f[i].v[j][2] = (m->f[i].v[j][2] / 1.f) * m->f[i].v[j][3];

            m->f[i].v[j] = vecxm(m->f[i].v[j], reperspMat);
            m->f[i].v[j][3] = 1.0f;

            m->f[i].v[j] = vecxm(m->f[i].v[j], lookAt);

            /* view space */
            m->f[i].v[j] = vecxm(m->f[i].v[j], mapWorld);
        }
    }

    /* At this Point triangles must be clipped against near and far plane. */
    vec4f plane_near_p = { 0.0f, 0.0f, NPlane },
        plane_near_n = { 0.0f, 0.0f, 1.0f };
    *m = clipp(*m, plane_near_p, plane_near_n);
    if (!m->f_indexes) {
        return 0;
    }

    vec4f plane_far_p = { 0.0f, 0.0f, FPlane},
        plane_far_n = { 0.0f, 0.0f, -1.0f };
    *m = clipp(*m, plane_far_p, plane_far_n);
    if (!m->f_indexes) {
        return 0;
    }

    for (int i = 0; i < m->f_indexes; i++) {
        for (int j = 0; j < 3; j++) {
            /* ppdiv */
            float w = m->f[i].v[j][3];
            m->f[i].v[j] /= w;
            m->f[i].v[j][3] = w;

            /* screen space */
            m->f[i].vt[j] /= m->f[i].v[j][3];

            m->f[i].v[j][0] = ((1.0f + m->f[i].v[j][0]) * (map_wa.width >> 1)) + 0.5;
            m->f[i].v[j][1] = ((1.0f + m->f[i].v[j][1]) * (map_wa.height >> 1)) + 0.5;
            m->f[i].v[j][2] = 1.0f / m->f[i].v[j][2];
            m->f[i].v[j][3] = 1.0f / m->f[i].v[j][3];
        }
    }

    vec4f plane_up_p = { 0.f, 0.f, 0.f },
          plane_up_n = { 0.f, 1.f, 0.f };
    *m = clipp(*m, plane_up_p, plane_up_n);
    if (!m->f_indexes)
        return 0;

    vec4f plane_down_p = { 0.f, map_wa.height - 1.f, 0.f },
          plane_down_n = { 0.f, -1.f, 0.f };
    *m = clipp(*m, plane_down_p, plane_down_n);
    if (!m->f_indexes)
        return 0;

    vec4f plane_left_p = { 0.f, 0.f, 0.f },
          plane_left_n = { 1.f, 0.f, 0.f };
    *m = clipp(*m, plane_left_p, plane_left_n);
    if (!m->f_indexes)
        return 0;

    vec4f plane_right_p = { map_wa.width - 1.f, 0.f, 0.f },
          plane_right_n = { -1.f, 0.f, 0.f };
    *m = clipp(*m, plane_right_p, plane_right_n);
    if (!m->f_indexes)
        return 0;

    return 1;
}
/* Rasterize given Mesh by passing them to the appropriate function. */
const static void maprasterize(const Mesh m) {
    point_buffer = map_buffer;
    point_depth_buffer = map_depth_buffer;
    point_attrib = &map_wa;
    point_mat = &mapLook;
    if (DEBUG == 1) {
        edgeMesh(m, m.material.basecolor);
    } else if (DEBUG == 2) {
        fillMesh(m);
    } else {
        texMesh(m);
    }
}


