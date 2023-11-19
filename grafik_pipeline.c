#include "headers/grafik_pipeline.h"

#include "headers/frustum_map.h"

const static void adoptdetail(Mesh *m);
const static void assemblyfaces(Mesh *m, const int len);
const static void ppdiv(Mesh *m, const int len);
const static Mesh bfculling(const Mesh m, const int len);
const static int viewtoscreen(Mesh *m, const int len);
const static void rasterize(const Mesh m);

/* Passes the scene Meshes throught the graphic pipeline. */
const void grafikPipeline(Scene s) {
    Mesh cache = { 0 };

    for (int i = 0; i < s.m_indexes; i++) {
        adoptdetail(&s.m[i]);

        initMesh(&cache, s.m[i]);

        cache.v = vecsarrayxm(s.m[i].v, s.m[i].v_indexes, worldMat);
        cache.n = vecsarrayxm(s.m[i].n, s.m[i].n_indexes, viewMat);

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
        vec4f plane_far_p = { 0.0f, 0.0f, FPlane},
              plane_far_n = { 0.0f, 0.0f, -1.0f };
        cache = clipp(cache, plane_far_p, plane_far_n);
        if (!cache.f_indexes) {
            releaseMesh(&cache);
            continue;
        }

        /* Applying perspective division. */
        if (!PROJECTIONVIEW)
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
    const int distance = len_vec(m->pivot - lookAt.m[3]);
    const int lcache = m->lvlofdetail;

    if ( (distance >= 0 && distance <= 200) && (m->lvlofdetail != 0) ) {
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


