#include "headers/shadow_pipeline.h"

const static void initfaceVerticesShadow(Mesh *m, const int len);
const static Mesh shadowculling(const Mesh c, const int len);
const static void shadowtoscreen(Mesh *m, const int len);

extern int HALFH, HALFW;
extern float FPlane, NPlane;
extern Light sunlight;
extern XWindowAttributes wa;
extern Mat4x4 orthoMat, lightMat;

const void shadowPipeline(Scene s) {
    Mesh cache = { 0 };
    Mat4x4 lm = lookat(sunlight.pos, sunlight.u, sunlight.v, sunlight.n);
    Mat4x4 Lview = inverse_mat(lm);
    lightMat = mxm(Lview, orthoMat);

    for (int i = 0; i < s.m_indexes; i++) {
        initMesh(&cache, s.m[i]);

        cache.v = meshxm(s.m[i].v, s.m[i].v_indexes, lightMat);
        cache.n = malloc(1);

        initfaceVerticesShadow(&cache, cache.f_indexes);

        /* At this Point triangles must be shadowclipped against near plane. */
        vec4f plane_near_p = { 0.f, 0.f, 0.01f, 0.f },
              plane_near_n = { 0.f, 0.f, 1.f, 0.f };
        cache = shadowclipp(cache, plane_near_p, plane_near_n);
    
        /* Applying perspective division. */
        if (cache.f_indexes) {

            /* Applying Backface culling before we proceed to full frustum shadowclipping. */
            cache = shadowculling(cache, cache.f_indexes);

            /* Sending to translation from NDC to Screen Coordinates. */
            shadowtoscreen(&cache, cache.f_indexes);

            createShadowmap(cache);
        }
        releaseMesh(&cache);
    }
}
/* Assosiates vertices coordinate values from vector array through indexes. */
const static void initfaceVerticesShadow(Mesh *m, const int len) {
    for (int i = 0; i < len; i++) {
        m->f[i].v[0] = m->v[m->f[i].a[0]];
        m->f[i].v[1] = m->v[m->f[i].b[0]];
        m->f[i].v[2] = m->v[m->f[i].c[0]];
    }
}
/* Backface culling.Discarding Triangles that should not be painted.Creating a new dynamic Mesh stucture Triangles array. */
const static Mesh shadowculling(const Mesh m, const int len) {
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
const static void shadowtoscreen(Mesh *m, const int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 3; j++) {

            m->f[i].vt[j] /= m->f[i].v[j][3];

            m->f[i].v[j][0] = ((1 + m->f[i].v[j][0]) * HALFW) + 0.5; /* adding 0.5 at this point so we can convert to integer at drawing stage. */
            m->f[i].v[j][1] = ((1 + m->f[i].v[j][1]) * HALFH) + 0.5; /* adding 0.5 at this point so we can convert to integer at drawing stage. */
            m->f[i].v[j][2] *= 0.5;//1.f / m->f[i].v[j][2];
            m->f[i].v[j][3] = 1.f / m->f[i].v[j][3];
        }
    }
    /* Far Plane shadowclipping and side shadowclipping. */
    vec4f plane_far_p = { 0.0, 0.0,  FPlane},
          plane_far_n = { 0.0, 0.0, 1.0 };
    *m = shadowclipp(*m, plane_far_p, plane_far_n);

    vec4f plane_up_p = { 0.0, 0.0, 0.0 },
          plane_up_n = { 0.0, 1.0, 0.0 };
    *m = shadowclipp(*m, plane_up_p, plane_up_n);

    vec4f plane_down_p = { 0.0, wa.height - 1.0, 0.0 },
          plane_down_n = { 0.0, -1.0, 0.0 };
    *m = shadowclipp(*m, plane_down_p, plane_down_n);

    vec4f plane_left_p = { 0.0, 0.0, 0.0 },
          plane_left_n = { 1.0, 0.0, 0.0 };
    *m = shadowclipp(*m, plane_left_p, plane_left_n);

    vec4f plane_right_p = { wa.width - 1.0, 0.0, 0.0 },
          plane_right_n = { -1.0, 0.0, 0.0 };
    *m = shadowclipp(*m, plane_right_p, plane_right_n);
}


