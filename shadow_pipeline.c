#include "headers/shadow_pipeline.h"

const static MeshShadowStepTwo assemblyfacesShadow(MeshShadowStepOne *m, unsigned int *indices, const int len);
// const static void shadowppdiv(Mesh *m, const int len);
// const static void clipptoview(Mesh *m, const int len);
// const static void setwone(Mesh *m, const int len);
const static MeshShadowStepTwo shadowculling(const MeshShadowStepTwo c, const int len);
const static int shadowtoscreen(MeshShadowStepTwo *m, const int len);
const static void createShadowmap(MeshShadowStepTwo *m, const unsigned int sm_index);
const static void shadowface(Shadowface *f, const Srt srt[], const unsigned int sm_index);
const static void releaseMeshShadowStepOne(MeshShadowStepOne *c);
const static void releaseMeshShadowStepTwo(MeshShadowStepTwo *c);
const static void initMeshShadowStepOne(MeshShadowStepOne *a, Mesh *b);
const static vec4i smmask = { 1, 2, 0, 3 };

const void shadowPipeline(Scene *s, const unsigned int sm_index) {
    MeshShadowStepOne cache_0 = { 0 };

    for (int i = 0; i < s->m_indexes; i++) {
        initMeshShadowStepOne(&cache_0, &s->m[i]);

        cache_0.v = setvecsarrayxm(cache_0.v, cache_0.v_indexes, ortholightMat[sm_index]);

        MeshShadowStepTwo cache_1 = assemblyfacesShadow(&cache_0, s->m[i].f, s->m[i].f_indexes);
        releaseMeshShadowStepOne(&cache_0);

        /* At this Point triangles must be shadowclipped against near plane. */
        vec4f plane_near_p = { 0.0f, 0.0f, NPlane },
                plane_near_n = { 0.0f, 0.0f, 1.0f };
        cache_1 = shadowclipp(&cache_1, plane_near_p, plane_near_n);
        if (!cache_1.f_indexes) {
            releaseMeshShadowStepTwo(&cache_1);
            continue;
        }

        /* Clipping against far Plane in View Space. */
        vec4f plane_far_p = { 0.0f, 0.0f,  FPlane},
                plane_far_n = { 0.0f, 0.0f, -1.0f };
        cache_1 = shadowclipp(&cache_1, plane_far_p, plane_far_n);
        if (!cache_1.f_indexes) {
            releaseMeshShadowStepTwo(&cache_1);
            continue;
        }

        /* Applying perspective division. */
        // if (!PROJECTIONVIEW)
            // shadowppdiv(&cache, cache.f_indexes);

        /* Applying Backface culling before we proceed to full frustum shadowclipping. */
        if (cache_1.cull)
            cache_1 = shadowculling(cache_1, cache_1.f_indexes);
        if (!cache_1.f_indexes) {
            releaseMeshShadowStepTwo(&cache_1);
            continue;
        }

        /* Retransform to View Space. */
        // if (!PROJECTIONVIEW)
            // clipptoview(&cache, cache.f_indexes);

        // cache.f = setfacesarrayxm(cache.f, cache.f_indexes, reperspMat);
        // setwone(&cache, cache.f_indexes);
        // cache.f = setfacesarrayxm(cache.f, cache.f_indexes, lm);
        // cache.f = setfacesarrayxm(cache.f, cache.f_indexes, inverse_mat(persplightMat));
        // cache.f = setfacesarrayxm(cache.f, cache.f_indexes, ortholightMat);

        /* Sending to translation from NDC to Screen Coordinates. */
        if (!shadowtoscreen(&cache_1, cache_1.f_indexes)) {
            releaseMeshShadowStepTwo(&cache_1);
            continue;
        }

        createShadowmap(&cache_1, sm_index);
        releaseMeshShadowStepTwo(&cache_1);
    }
}
/* Assosiates vertices coordinate values from vector array through indexes. */
const static MeshShadowStepTwo assemblyfacesShadow(MeshShadowStepOne *m, unsigned int *indices, const int len) {
    MeshShadowStepTwo r = { 0 };
    r.f_indexes = len / 9;
    r.f = malloc(sizeof(Shadowface) * r.f_indexes);

    int index = 0;
    for (int i = 0; i < len; i += 9) {
        r.f[index].v[0] = m->v[indices[i]];
        r.f[index].v[1] = m->v[indices[i + 3]];
        r.f[index].v[2] = m->v[indices[i + 6]];
        index++;
    }

    r.cull = m->cull;
    return r;
}
// /* Perspective division. */
// const static void shadowppdiv(Mesh *m, const int len) {
//     for (int i = 0; i < len; i++) {
//         for (int j = 0; j < 3; j++) {
//             float w = m->f[i].v[j][3];
//             m->f[i].v[j] /= w;
//             m->f[i].v[j][3] = w;
//         }
//     }
// }
// /* Perspective division. */
// const static void clipptoview(Mesh *m, const int len) {
//     for (int i = 0; i < len; i++) {
//         for (int j = 0; j < 3; j++) {
//             float w = m->f[i].v[j][3];
//             m->f[i].v[j] *= w;
//             m->f[i].v[j][3] = w;
//         }
//     }
// }
// /* Perspective division. */
// const static void setwone(Mesh *m, const int len) {
//     for (int i = 0; i < len; i++) {
//         for (int j = 0; j < 3; j++) {
//             m->f[i].v[j][3] = 1.f;
//         }
//     }
// }
/* Backface culling.Discarding Triangles that should not be painted.Creating a new dynamic Mesh stucture Triangles array. */
const static MeshShadowStepTwo shadowculling(const MeshShadowStepTwo m, const int len) {
    MeshShadowStepTwo r = m;
    size_t size = sizeof(Shadowface);
    int counter = 1;
    int index = 0;
    r.f = malloc(size);
    if (!r.f)
        fprintf(stderr, "Could not allocate memory - bfculling() - malloc\n");

    for (int i = 0; i < len; i++) {
        if (shadow_winding(m.f[i]) > 0.0f) {
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
const static int shadowtoscreen(MeshShadowStepTwo *m, const int len) {
    for (int i = 0; i < len; i++) {
        // m->f[i].fn = norm_vec(triangle_cp(m->f[i]));
        for (int j = 0; j < 3; j++) {
            m->f[i].v[j][0] = ((1 + m->f[i].v[j][0]) * HALFW) + 0.5; /* adding 0.5 at this point so we can convert to integer at drawing stage. */
            m->f[i].v[j][1] = ((1 + m->f[i].v[j][1]) * HALFH) + 0.5; /* adding 0.5 at this point so we can convert to integer at drawing stage. */
            m->f[i].v[j][2] = 1.f / m->f[i].v[j][2];
            // m->f[i].v[j][3] = 1.f / m->f[i].v[j][3];
        }
    }

    vec4f plane_up_p = { 0.0, 0.0, 0.0 },
          plane_up_n = { 0.0, 1.0, 0.0 };
    *m = shadowclipp(m, plane_up_p, plane_up_n);
    if (!m->f_indexes)
        return 0;

    vec4f plane_down_p = { 0.0, main_wa.height - 1.0, 0.0 },
          plane_down_n = { 0.0, -1.0, 0.0 };
    *m = shadowclipp(m, plane_down_p, plane_down_n);
    if (!m->f_indexes)
        return 0;

    vec4f plane_left_p = { 0.0, 0.0, 0.0 },
          plane_left_n = { 1.0, 0.0, 0.0 };
    *m = shadowclipp(m, plane_left_p, plane_left_n);
    if (!m->f_indexes)
        return 0;

    vec4f plane_right_p = { main_wa.width - 1.0, 0.0, 0.0 },
          plane_right_n = { -1.0, 0.0, 0.0 };
    *m = shadowclipp(m, plane_right_p, plane_right_n);
    if (!m->f_indexes)
        return 0;

    return 1;
}
const void createShadowmap(MeshShadowStepTwo *m, const unsigned int sm_index) {
    for (int c = 0; c < m->f_indexes; c++) {
        /* Creating 2Arrays for X and Y values to sort them. */
        Srt srt[3] = {
            { .y =  m->f[c].v[0][1], .index = 0},
            { .y =  m->f[c].v[1][1], .index = 1},
            { .y =  m->f[c].v[2][1], .index = 2}
        };

        /* Sorting the values from smaller to larger y. */
        Srt temp;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                if (srt[i].y < srt[j].y) {
                    temp = srt[j];
                    srt[j] = srt[i];
                    srt[i] = temp;
                }

        shadowface(&m->f[c], srt, sm_index);
    }
}
const void shadowface(Shadowface *f, const Srt srt[], const unsigned int sm_index) {
    vec4i xs = { f->v[srt[0].index][0], f->v[srt[1].index][0], f->v[srt[2].index][0], 0 };
    vec4i ys = { f->v[srt[0].index][1], f->v[srt[1].index][1], f->v[srt[2].index][1], 0 };
    vec4f zs = { f->v[srt[0].index][2], f->v[srt[1].index][2], f->v[srt[2].index][2], 0 };
    vec4i xmx = __builtin_shuffle(xs, smmask) - xs;
    vec4i ymy = __builtin_shuffle(ys, smmask) - ys;
    vec4f zmz = __builtin_shuffle(zs, smmask) - zs;

    const int orient = (xmx[0] * ymy[2]) - (ymy[0] * xmx[2]);
    float ma = (float)xmx[0] / ymy[0];
    float mb = (float)xmx[2] / ymy[2];
    float za = zmz[0] / ymy[0];
    float zb = zmz[2] / ymy[2];
    if (orient < 0) {
        swap(&ma, &mb, 4);
        swap(&za, &zb, 4);
    }

    // vec4f lightpos = norm_vec(vecxm(sunlight.pos, lightMat));
    // float dot = dot_product(lightpos, f->fn);
    // printf("dot_product: %f\n", dot);
    // if (dot > 0 && dot < 0.00025)
    //     return;

    const int y_start = ys[0];
    const int y_end1 = ys[1];
    const int y_end2 = ys[2];

    int yA = 0;
    if (ymy[0] != 0)
        for (int y = y_start; y < y_end1; y++) {
            const int padySB = y * main_wa.width;

            const int x_start = (ma * yA) + xs[0];
            const int x_end = (mb * yA) + xs[0];

            const float z1 = (za * yA) + zs[0];
            const float z2 = (zb * yA) + zs[0];

            const float xexs = x_end - x_start;
            const float z2z1 = z2 - z1;
            float xxs = 0.0;
            for (int x = x_start; x < x_end; x++) {

                const float barycentric = xxs / xexs;
                const float depthZ = z1 + (barycentric * z2z1);
                
                const int padxSB = padySB + x;
                if ( depthZ > shadow_buffer[sm_index][padxSB] ) {

                    shadow_buffer[sm_index][padxSB] = depthZ;
                }
                xxs += 1.0;
            }
            yA++;
        }

    if (ymy[1] == 0)
        return;

    ma = (float)xmx[1] / ymy[1];
    mb = (float)xmx[2] / ymy[2];
    za = zmz[1] / ymy[1];
    zb = zmz[2] / ymy[2];
    if (orient < 0) {
        swap(&ma, &mb, 4);
        swap(&za, &zb, 4);
    }

    int yB = -ymy[1];
    for (int y = y_end1; y < y_end2; y++) {
        const int padySB = y * main_wa.width;

        const int x_start = (ma * yB) + xs[2];
        const int x_end = (mb * yB) + xs[2];

        const float z1 = (za * yB) + zs[2];
        const float z2 = (zb * yB) + zs[2];

        const float xexs = x_end - x_start;
        const float z2z1 = z2 - z1;
        float xxs = 0.0;
        for (int x = x_start; x < x_end; x++) {

            const float barycentric = xxs / xexs;
            const float depthZ = z1 + (barycentric * z2z1);

            const int padxSB = padySB + x;
            if ( depthZ > shadow_buffer[sm_index][padxSB] ) {

                shadow_buffer[sm_index][padxSB] = depthZ;
            }
            xxs += 1.0;
        }
        yB++;
    }
}
const float shadowTest(vec4f frag, vec4f nml) {
    int sm_index;
    if (frag[2] >= 0.f && frag[2] <= 100.f)
        sm_index = 0;
    else if (frag[2] > 100.f && frag[2] <= 300.f)
        sm_index = 1;
    else if (frag[2] > 300.f)
        sm_index = 2;

    // float dot = dot_product(norm_vec(sunlight.newPos), nml);
    // if ( dot > -0.2 && dot < 0.2 )
    //     return 0;
    // printf("dot: %f\n", dot);
    /* Transform to Model space coordinates. */
    // logVec4f(norm_vec(sunlight.newPos));
    frag[3] = 1.f;
    frag = vecxm(frag, *point_mat);

    /* Transform to Light space coordinates. */
    frag = vecxm(frag, ortholightMat[sm_index]);

    float x = frag[0];
    float y = frag[1];
    float z = frag[2];

    /* Transform to Screen space coordinates. */
    x = (1.0 + x) * (main_wa.width >> 1);
    if ( (x < 0) || (x >= main_wa.width) )
        return 1.f;

    y = (1.0 + y) * (main_wa.height >> 1);
    if ( (y < 0) || (y >= main_wa.height) )
        return 1.f;

    z = 1.f / z;

    if ( z < shadow_buffer[sm_index][((int)y * main_wa.width) + (int)x] + shadow_bias)
        return 0.f;

    return 1.f;
}
/* Releases all members of the given inside graphic pipeline lvl 1 Mesh. */
const void releaseMeshShadowStepOne(MeshShadowStepOne *c) {
    free(c->v);
}
/* Releases all members of the given inside graphic pipeline lvl 2 Mesh. */
const void releaseMeshShadowStepTwo(MeshShadowStepTwo *c) {
    free(c->f);
}
/* Initializing Mesh a from Mesh b. */
const void initMeshShadowStepOne(MeshShadowStepOne *a, Mesh *b) {
    size_t vsize = sizeof(vec4f) * b->v_indexes;

    a->v = malloc(vsize);
    memcpy(a->v, b->v, vsize);
    a->v_indexes = b->v_indexes;

    a->cull = b->cull;
}


