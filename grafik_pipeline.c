#include "headers/grafik_pipeline.h"

#include "headers/frustum_map.h"

const static void adoptdetail(Mesh *m);
const static MeshStepTwo assemblyfaces(MeshStepOne *m, unsigned int *indices, const int len);
const static void ppdiv(MeshStepTwo *m, const int len);
const static MeshStepTwo bfculling(const MeshStepTwo m, const int len);
const static int viewtoscreen(MeshStepTwo *m, const int len);
const static void rasterize(MeshStepTwo *m, Material *mtr);

/* Passes the scene Meshes throught the graphic pipeline. */
const void grafikPipeline(Scene *s) {
    MeshStepOne cache_0 = { 0 };
    // Scene frustum;
    // size_t mesh_size = sizeof(Mesh);
    // frustum.m = malloc(mesh_size);
    // int index = 0, dynamic_inc = 1;

    for (int i = 0; i < s->m_indexes; i++) {
        adoptdetail(&s->m[i]);

        initMeshStepOne(&cache_0, &s->m[i]);

        cache_0.v = setvecsarrayxm(cache_0.v, cache_0.v_indexes, worldMat);
        cache_0.n = setvecsarrayxm(cache_0.n, cache_0.n_indexes, viewMat);

        /* Assembly and create the faces from the mesh vertices, normals and texture arrays, through the indexes. */
        MeshStepTwo cache_1 = assemblyfaces(&cache_0, s->m[i].f, s->m[i].f_indexes);
        releaseMeshStepOne(&cache_0);

        /* Clipping against near Plane in View Space. */
        vec4f plane_near_p = { 0.0f, 0.0f, NPlane },
                plane_near_n = { 0.0f, 0.0f, 1.0f };
        cache_1 = clipp(&cache_1, plane_near_p, plane_near_n);
        if (!cache_1.f_indexes) {
            releaseMeshStepTwo(&cache_1);
            continue;
        }

        /* Clipping against far Plane in View Space. */
        vec4f plane_far_p = { 0.0f, 0.0f, FPlane},
              plane_far_n = { 0.0f, 0.0f, -1.0f };
        cache_1 = clipp(&cache_1, plane_far_p, plane_far_n);
        if (!cache_1.f_indexes) {
            releaseMeshStepTwo(&cache_1);
            continue;
        }

        /* Applying perspective division. */
        if (!PROJECTIONVIEW)
            ppdiv(&cache_1, cache_1.f_indexes);

        /* Applying Backface culling before we proceed to Screen Space transformation and view Port clipping. */
        if (cache_1.cull)
            cache_1 = bfculling(cache_1, cache_1.f_indexes);
        if (!cache_1.f_indexes) {
            releaseMeshStepTwo(&cache_1);
            continue;
        }

        /* Sending to translation from NDC to Screen Coordinates. */
        if (!viewtoscreen(&cache_1, cache_1.f_indexes)) {
            releaseMeshStepTwo(&cache_1);
            continue;
        }

        // frustum.m = realloc(frustum.m, dynamic_inc * mesh_size);
        // frustum.m[index] = cache_0;
        // index++;
        // dynamic_inc++;

        rasterize(&cache_1, &s->m[i].material);
        releaseMeshStepTwo(&cache_1);
    }
    // frustum.m_indexes = index;
    // if (frustum.m) {
    //     mapPipeline(&frustum);
    //     releaseScene(&frustum);
    // }
}
const static void adoptdetail(Mesh *m) {
    const int distance = len_vec(m->pivot - lookAt.m[3]);
    const int lcache_0 = m->lvlofdetail;

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

    if (strcmp(m->material.texlvl[lcache_0], m->material.texlvl[m->lvlofdetail]) != 0) {
        loadtexture(m);
    }
}
/* Assosiates vertices coordinate values from vector array through indexes. */
const static MeshStepTwo assemblyfaces(MeshStepOne *m, unsigned int *indices, const int len) {
    MeshStepTwo r = { 0 };
    r.f_indexes = len / 9;
    r.f = malloc(sizeof(face) * r.f_indexes);

    int index = 0;
    for (int i = 0; i < len; i += 9) {
        r.f[index].v[0] = m->v[indices[i]];
        r.f[index].v[1] = m->v[indices[i + 3]];
        r.f[index].v[2] = m->v[indices[i + 6]];

        r.f[index].vt[0] = m->t[indices[i + 1]];
        r.f[index].vt[1] = m->t[indices[i + 4]];
        r.f[index].vt[2] = m->t[indices[i + 7]];

        r.f[index].vn[0] = m->n[indices[i + 2]];
        r.f[index].vn[1] = m->n[indices[i + 5]];
        r.f[index].vn[2] = m->n[indices[i + 8]];
        index++;
    }

    r.cull = m->cull;
    r.lvlofdetail = m->lvlofdetail;
    return r;
}
/* Perspective division. */
const static void ppdiv(MeshStepTwo *m, const int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 3; j++) {
            float w = m->f[i].v[j][3];
            m->f[i].v[j] /= w;
            m->f[i].v[j][3] = w;
        }
    }
}
/* Backface culling.Discarding Triangles that should not be painted.Creating a new dynamic Mesh stucture Triangles array. */
const static MeshStepTwo bfculling(const MeshStepTwo m, const int len) {
    MeshStepTwo r = m;
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
const static int viewtoscreen(MeshStepTwo *m, const int len) {
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
    *m = clipp(m, plane_up_p, plane_up_n);
    if(!m->f_indexes)
        return 0;

    vec4f plane_down_p = { 0.0f, main_wa.height - 1.0f, 0.0f },
        plane_down_n = { 0.0f, -1.0f, 0.0f };
    *m = clipp(m, plane_down_p, plane_down_n);
    if(!m->f_indexes)
        return 0;

    vec4f plane_left_p = { 0.0f, 0.0f, 0.0f },
        plane_left_n = { 1.0f, 0.0f, 0.0f };
    *m = clipp(m, plane_left_p, plane_left_n);
    if(!m->f_indexes)
        return 0;

    vec4f plane_right_p = { main_wa.width - 1.0f, 0.0f, 0.0f },
        plane_right_n = { -1.0f, 0.0f, 0.0f };
    *m = clipp(m, plane_right_p, plane_right_n);
    if(!m->f_indexes)
        return 0;

    return 1;
}
/* Rasterize given Mesh by passing them to the appropriate function. */
const static void rasterize(MeshStepTwo *m, Material *mtr) {
    point_buffer = frame_buffer;
    point_depth_buffer = main_depth_buffer;
    point_attrib = &main_wa;
    point_mat = &lookAt;

    if (DEBUG == 1) {
        edgeMesh(m, mtr->basecolor);
    } else if (DEBUG == 2) {
        fillMesh(m, mtr);
    } else {
        texMesh(m, mtr);
    }
}


