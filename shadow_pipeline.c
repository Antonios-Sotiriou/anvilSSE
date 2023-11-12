#include "headers/shadow_pipeline.h"

#include "headers/logging.h"

const static void assemblyfacesShadow(Mesh *m, const int len);
const static void shadowppdiv(Mesh *m, const int len);
const static void clipptoview(Mesh *m, const int len);
const static Mesh shadowculling(const Mesh c, const int len);
const static int shadowtoscreen(Mesh *m, const int len);
const static void createShadowmap(Mesh m);
const static void shadowface(const face f, const Srt srt[]);
const static vec4i sdmask = { 1, 2, 0, 3 };
const static void setwone(Mesh *m, const int len);
const void shadowPipeline(Scene s) {
    Mesh cache = { 0 };
    for (int i = 0; i < s.m_indexes; i++) {
        initMesh(&cache, s.m[i]);

        cache.v = vecsarrayxm(s.m[i].v, s.m[i].v_indexes, ortholightMat);
        cache.n = malloc(1);
        assemblyfacesShadow(&cache, cache.f_indexes);

        /* At this Point triangles must be shadowclipped against near plane. */
        vec4f plane_near_p = { 0.0f, 0.0f, NPlane },
                plane_near_n = { 0.0f, 0.0f, 1.0f };
        cache = shadowclipp(cache, plane_near_p, plane_near_n);
        if (!cache.f_indexes) {
            releaseMesh(&cache);
            continue;
        }

        /* Clipping against far Plane in View Space. */
        vec4f plane_far_p = { 0.0f, 0.0f,  FPlane},
                plane_far_n = { 0.0f, 0.0f, -1.0f };
        cache = shadowclipp(cache, plane_far_p, plane_far_n);
        if (!cache.f_indexes) {
            releaseMesh(&cache);
            continue;
        }

        /* Applying perspective division. */
        // if (!PROJECTIONVIEW)
            // shadowppdiv(&cache, cache.f_indexes);

        /* Applying Backface culling before we proceed to full frustum shadowclipping. */
        if (cache.cull)
            cache = shadowculling(cache, cache.f_indexes);
        if (!cache.f_indexes) {
            releaseMesh(&cache);
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
        if (!shadowtoscreen(&cache, cache.f_indexes)) {
            releaseMesh(&cache);
            continue;
        }

        createShadowmap(cache);
        releaseMesh(&cache);
    }
}
/* Assosiates vertices coordinate values from vector array through indexes. */
const static void assemblyfacesShadow(Mesh *m, const int len) {
    for (int i = 0; i < len; i++) {
        m->f[i].v[0] = m->v[m->f[i].a[0]];
        m->f[i].v[1] = m->v[m->f[i].b[0]];
        m->f[i].v[2] = m->v[m->f[i].c[0]];
    }
}
/* Perspective division. */
const static void shadowppdiv(Mesh *m, const int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 3; j++) {
            float w = m->f[i].v[j][3];
            m->f[i].v[j] /= w;
            m->f[i].v[j][3] = w;
        }
    }
}
/* Perspective division. */
const static void clipptoview(Mesh *m, const int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 3; j++) {
            float w = m->f[i].v[j][3];
            m->f[i].v[j] *= w;
            m->f[i].v[j][3] = w;
        }
    }
}
/* Perspective division. */
const static void setwone(Mesh *m, const int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 3; j++) {
            m->f[i].v[j][3] = 1.f;
        }
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
        if (winding(m.f[i]) > 0.0f) {
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
const static int shadowtoscreen(Mesh *m, const int len) {
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
    *m = shadowclipp(*m, plane_up_p, plane_up_n);
    if (!m->f_indexes)
        return 0;

    vec4f plane_down_p = { 0.0, wa.height - 1.0, 0.0 },
          plane_down_n = { 0.0, -1.0, 0.0 };
    *m = shadowclipp(*m, plane_down_p, plane_down_n);
    if (!m->f_indexes)
        return 0;

    vec4f plane_left_p = { 0.0, 0.0, 0.0 },
          plane_left_n = { 1.0, 0.0, 0.0 };
    *m = shadowclipp(*m, plane_left_p, plane_left_n);
    if (!m->f_indexes)
        return 0;

    vec4f plane_right_p = { wa.width - 1.0, 0.0, 0.0 },
          plane_right_n = { -1.0, 0.0, 0.0 };
    *m = shadowclipp(*m, plane_right_p, plane_right_n);
    if (!m->f_indexes)
        return 0;

    return 1;
}
const void createShadowmap(Mesh m) {
    for (int c = 0; c < m.f_indexes; c++) {
        /* Creating 2Arrays for X and Y values to sort them. */
        Srt srt[3] = {
            { .y =  m.f[c].v[0][1], .index = 0},
            { .y =  m.f[c].v[1][1], .index = 1},
            { .y =  m.f[c].v[2][1], .index = 2}
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

        shadowface(m.f[c], srt);
    }
}
const void shadowface(const face f, const Srt srt[]) {
    vec4i xs = { f.v[srt[0].index][0], f.v[srt[1].index][0], f.v[srt[2].index][0], 0 };
    vec4i ys = { f.v[srt[0].index][1], f.v[srt[1].index][1], f.v[srt[2].index][1], 0 };
    vec4f zs = { f.v[srt[0].index][2], f.v[srt[1].index][2], f.v[srt[2].index][2], 0 };
    vec4i xmx = __builtin_shuffle(xs, sdmask) - xs;
    vec4i ymy = __builtin_shuffle(ys, sdmask) - ys;
    vec4f zmz = __builtin_shuffle(zs, sdmask) - zs;

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
    // float dot = dot_product(lightpos, f.fn);
    // printf("dot_product: %f\n", dot);
    // if (dot > 0 && dot < 0.00025)
    //     return;

    const int y_start = ys[0];
    const int y_end1 = ys[1];
    const int y_end2 = ys[2];

    int yA = 0;
    if (ymy[0] != 0)
        for (int y = y_start; y < y_end1; y++) {
            const int padySB = y * wa.width;

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
                if ( depthZ > shadow_buffer[padxSB] ) {

                    shadow_buffer[padxSB] = depthZ;
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
        const int padySB = y * wa.width;

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
            if ( depthZ > shadow_buffer[padxSB] ) {

                shadow_buffer[padxSB] = depthZ;
            }
            xxs += 1.0;
        }
        yB++;
    }
}
const int shadowTest(vec4f frag) {
    frag[3] = 1.f;
    /* Transform to Model space coordinates. */
    frag = vecxm(frag, lookAt);

    /* Transform to Light space coordinates. */
    frag = vecxm(frag, ortholightMat);
    // float w = frag[3];
    // frag /= w;
    // frag[3] = w;

    float x = frag[0];
    float y = frag[1];
    float z = frag[2];

    /* Transform to Screen space coordinates. */
    x = (1.0 + x) * HALFW;
    if ( (x < 0) || (x >= wa.width) )
        return 0;

    y = (1.0 + y) * HALFH;
    if ( (y < 0) || (y >= wa.height) )
        return 0;

    z = 1.f / z;
    if ( z < shadow_buffer[((int)y * wa.width) + (int)x] + shadow_bias)
        return 1;

    return 0;
}


