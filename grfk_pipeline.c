#include "headers/grfk_pipeline.h"

extern int HALFH, HALFW, DEBUG;
extern float FPlane, NPlane;;
extern Light sunlight;
extern XWindowAttributes wa;
extern Mat4x4 viewMat, worldMat, orthoMat, lightMat;

const void shadowPipeline(Scene s) {
    Mesh cache = { 0 };
    Mat4x4 lm = lookat(sunlight.newPos, sunlight.u, sunlight.v, sunlight.n);
    Mat4x4 Lview = inverse_mat(lm);
    lightMat = mxm(Lview, orthoMat);

    for (int i = 0; i < s.m_indexes; i++) {
        initMesh(&cache, s.m[i]);

        cache.v = meshxm(s.m[i].v, s.m[i].v_indexes, lightMat);
        cache.n = malloc(1);

        initfaceVerticesShadow(&cache, cache.f_indexes);

        /* At this Point triangles must be clipped against near plane. */
        vec4f plane_near_p = { 0.0, 0.0, NPlane, 0.0 },
              plane_near_n = { 0.0, 0.0, 1.0, 0.0 };
        cache = clipp(cache, plane_near_p, plane_near_n);
    
        /* Applying perspective division. */
        if (cache.f_indexes) {

            /* Applying Backface culling before we proceed to full frustum clipping. */
            cache = bfculling(cache, cache.f_indexes);

            /* Sending to translation from NDC to Screen Coordinates. */
            viewtoscreen(&cache, cache.f_indexes);
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
/* Passes the scene Meshes throught the graphic pipeline. */
const void grfkPipeline(Scene s) {
    Mesh cache = { 0 };

    for (int i = 0; i < s.m_indexes; i++) {
        initMesh(&cache, s.m[i]);

        cache.v = meshxm(s.m[i].v, s.m[i].v_indexes, worldMat);
        cache.n = normalsxm(s.m[i].n, s.m[i].n_indexes, viewMat);

        initfaceVertices(&cache, cache.f_indexes);

        /* At this Point triangles must be clipped against near plane. */
        vec4f plane_near_p = { 0.0, 0.0, NPlane, 0.0 },
              plane_near_n = { 0.0, 0.0, 1.0, 0.0 };
        cache = clipp(cache, plane_near_p, plane_near_n);
    
        /* Applying perspective division. */
        if (cache.f_indexes) {
            ppdiv(&cache, cache.f_indexes);

            /* Applying Backface culling before we proceed to full frustum clipping. */
            cache = bfculling(cache, cache.f_indexes);

            /* Sending to translation from NDC to Screen Coordinates. */
            viewtoscreen(&cache, cache.f_indexes);
            rasterize(cache);
        }
        releaseMesh(&cache);
    }
}
/* Assosiates vertices coordinate values from vector array through indexes. */
const static void initfaceVertices(Mesh *m, const int len) {
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
            m->f[i].v[j] = m->f[i].v[j] / w;
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
const static void viewtoscreen(Mesh *m, const int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 3; j++) {
            m->f[i].v[j][0] = ((++m->f[i].v[j][0]) * HALFW) + 0.5; /* adding 0.5 at this point so we can convert to integer at drawing stage. */
            m->f[i].v[j][1] = ((++m->f[i].v[j][1]) * HALFH) + 0.5; /* adding 0.5 at this point so we can convert to integer at drawing stage. */
            m->f[i].v[j][2] *= 0.5;
            m->f[i].v[j][3] = 1 / m->f[i].v[j][3];
        }
    }
    /* Far Plane clipping and side clipping. */
    vec4f plane_far_p = { 0.0, 0.0,  FPlane},
          plane_far_n = { 0.0, 0.0, 1.0 };
    *m = clipp(*m, plane_far_p, plane_far_n);

    vec4f plane_right_p = { wa.width - 1.0, 0.0, 0.0 },
          plane_right_n = { -1.0, 0.0, 0.0 };
    *m = clipp(*m, plane_right_p, plane_right_n);

    vec4f plane_down_p = { 0.0, wa.height - 1.0, 0.0 },
          plane_down_n = { 0.0, -1.0, 0.0 };
    *m = clipp(*m, plane_down_p, plane_down_n);

    vec4f plane_left_p = { 0.0, 0.0, 0.0 },
          plane_left_n = { 1.0, 0.0, 0.0 };
    *m = clipp(*m, plane_left_p, plane_left_n);

    vec4f plane_up_p = { 0.0, 0.0, 0.0 },
          plane_up_n = { 0.0, 1.0, 0.0 };
    *m = clipp(*m, plane_up_p, plane_up_n);
}
/* Rasterize given Mesh by passing them to the appropriate function. */
const static void rasterize(const Mesh m) {
    if (DEBUG == 1) {
        edgeMesh(m, m.material.basecolor);
    } else {
        fillMesh(m);
    } //else {
    //     texMesh(m);
    // }
}

