#include "headers/height_pipeline.h"

const static MeshHeightStepTwo assemblyfacesHeight(MeshHeightStepOne *m, unsigned int *indices, const int len);
const static MeshHeightStepTwo heightculling(const MeshHeightStepTwo m, const int len);
const static float height_winding(const Heightface f);
const static void heightppdiv(MeshHeightStepTwo *m, const int len);
const static int heighttoscreen(MeshHeightStepTwo *m, const int len);
const static void createHeightmap(MeshHeightStepTwo *m);
const static void heightface(Heightface *f, const Srt srt[]);
const static void releaseMeshHeightStepOne(MeshHeightStepOne *c);
const static void releaseMeshHeightStepTwo(MeshHeightStepTwo *c);
const static void initMeshHeightStepOne(MeshHeightStepOne *a, Mesh *b);
const static vec4i hmask = { 1, 2, 0, 3 };
Mat4x4 viewHeight, heightWorld;

const void heightPipeline(Scene *s) {
    MeshHeightStepOne cache_0 = { 0 };

    viewHeight = inverse_mat(lookat(birdview[0] + camera[0], birdview[1], birdview[2], birdview[3]));
    heightWorld = mxm(viewHeight, perspMat);

    for (int i = 0; i < s->m_indexes; i++) {
        // if (s->m[i].type == Terrain) {
            initMeshHeightStepOne(&cache_0, &s->m[i]);

            cache_0.v = setvecsarrayxm(cache_0.v, cache_0.v_indexes, heightWorld);
            cache_0.n = setvecsarrayxm(cache_0.n, cache_0.n_indexes, viewHeight);

            /* Assembly and create the faces from the mesh vertices, normals and texture arrays, through the indexes. */
            MeshHeightStepTwo cache_1 = assemblyfacesHeight(&cache_0, s->m[i].f, s->m[i].f_indexes);
            releaseMeshHeightStepOne(&cache_0);

            /* Clipping against near Plane in View Space. */
            vec4f plane_near_p = { 0.0f, 0.0f, NPlane },
                    plane_near_n = { 0.0f, 0.0f, 1.0f };
            cache_1 = heightclipp(&cache_1, plane_near_p, plane_near_n);
            if (!cache_1.f_indexes) {
                releaseMeshHeightStepTwo(&cache_1);
                continue;
            }

            /* Clipping against far Plane in View Space. */
            vec4f plane_far_p = { 0.0f, 0.0f, FPlane},
                plane_far_n = { 0.0f, 0.0f, -1.0f };
            cache_1 = heightclipp(&cache_1, plane_far_p, plane_far_n);
            if (!cache_1.f_indexes) {
                releaseMeshHeightStepTwo(&cache_1);
                continue;
            }

            /* Applying perspective division. */
            // if (!PROJECTIONVIEW)
            heightppdiv(&cache_1, cache_1.f_indexes);

            /* Applying Backface culling before we proceed to Screen Space transformation and view Port clipping. */
            if (cache_1.cull)
                cache_1 = heightculling(cache_1, cache_1.f_indexes);
            if (!cache_1.f_indexes) {
                releaseMeshHeightStepTwo(&cache_1);
                continue;
            }

            /* Sending to translation from NDC to Screen Coordinates. */
            if (!heighttoscreen(&cache_1, cache_1.f_indexes)) {
                releaseMeshHeightStepTwo(&cache_1);
                continue;
            }

            createHeightmap(&cache_1);
            releaseMeshHeightStepTwo(&cache_1);
        // }
    }
}
/* Assosiates vertices coordinate values from vector array through indexes. */
const static MeshHeightStepTwo assemblyfacesHeight(MeshHeightStepOne *m, unsigned int *indices, const int len) {
    MeshHeightStepTwo r = { 0 };
    r.f_indexes = len / 9;
    r.f = malloc(sizeof(Heightface) * r.f_indexes);

    int index = 0;
    for (int i = 0; i < len; i += 9) {
        r.f[index].v[0] = m->v[indices[i]];
        r.f[index].v[1] = m->v[indices[i + 3]];
        r.f[index].v[2] = m->v[indices[i + 6]];

        r.f[index].vn[0] = m->v[indices[i + 2]];
        r.f[index].vn[1] = m->v[indices[i + 5]];
        r.f[index].vn[2] = m->v[indices[i + 8]];
        index++;
    }

    r.cull = m->cull;
    return r;
}
/* Backface culling.Discarding Triangles that should not be painted.Creating a new dynamic Mesh stucture Triangles array. */
const static MeshHeightStepTwo heightculling(const MeshHeightStepTwo m, const int len) {
    MeshHeightStepTwo r = m;
    size_t size = sizeof(Heightface);
    int counter = 1;
    int index = 0;
    r.f = malloc(size);
    if (!r.f)
        fprintf(stderr, "Could not allocate memory - bfculling() - malloc\n");

    for (int i = 0; i < len; i++) {
        if (height_winding(m.f[i]) > 0.00) {
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
/* Identifies if the Vectors are in clockwise order < CW > or not < CCW >. */
const static float height_winding(const Heightface f) {
    vec4f xs = { f.v[0][0], f.v[1][0], f.v[2][0], 0.0f };
    vec4f ys = { f.v[0][1], f.v[1][1], f.v[2][1], 0.0f };

    vec4f r = xs * __builtin_shuffle(ys, hmask) - ys * __builtin_shuffle(xs, hmask);
    return r[0] + r[1] + r[2];
}
/* Perspective division. */
const static void heightppdiv(MeshHeightStepTwo *m, const int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 3; j++) {
            float w = m->f[i].v[j][3];
            m->f[i].v[j] /= w;
            m->f[i].v[j][3] = w;
        }
    }
}
/* Translates the Mesh's Triangles from world to Screen Coordinates. */
const static int heighttoscreen(MeshHeightStepTwo *m, const int len) {
    for (int i = 0; i < len; i++) {
        // m->f[i].fn = norm_vec(triangle_cp(m->f[i]));
        for (int j = 0; j < 3; j++) {
            m->f[i].v[j][0] = ((1 + m->f[i].v[j][0]) * HALFW) + 0.5; /* adding 0.5 at this point so we can convert to integer at drawing stage. */
            m->f[i].v[j][1] = ((1 + m->f[i].v[j][1]) * HALFH) + 0.5; /* adding 0.5 at this point so we can convert to integer at drawing stage. */
            // m->f[i].v[j][2] = 1.f / m->f[i].v[j][2];
            m->f[i].v[j][3] = 1.f / m->f[i].v[j][3];
        }
    }

    vec4f plane_up_p = { 0.0, 0.0, 0.0 },
          plane_up_n = { 0.0, 1.0, 0.0 };
    *m = heightclipp(m, plane_up_p, plane_up_n);
    if (!m->f_indexes)
        return 0;

    vec4f plane_down_p = { 0.0, main_wa.height - 1.0, 0.0 },
          plane_down_n = { 0.0, -1.0, 0.0 };
    *m = heightclipp(m, plane_down_p, plane_down_n);
    if (!m->f_indexes)
        return 0;

    vec4f plane_left_p = { 0.0, 0.0, 0.0 },
          plane_left_n = { 1.0, 0.0, 0.0 };
    *m = heightclipp(m, plane_left_p, plane_left_n);
    if (!m->f_indexes)
        return 0;

    vec4f plane_right_p = { main_wa.width - 1.0, 0.0, 0.0 },
          plane_right_n = { -1.0, 0.0, 0.0 };
    *m = heightclipp(m, plane_right_p, plane_right_n);
    if (!m->f_indexes)
        return 0;

    return 1;
}
const void createHeightmap(MeshHeightStepTwo *m) {
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

        heightface(&m->f[c], srt);
    }
}
const void heightface(Heightface *f, const Srt srt[]) {
    vec4i xs = { f->v[srt[0].index][0], f->v[srt[1].index][0], f->v[srt[2].index][0], 0 };
    vec4i ys = { f->v[srt[0].index][1], f->v[srt[1].index][1], f->v[srt[2].index][1], 0 };
    vec4f zs = { f->v[srt[0].index][3], f->v[srt[1].index][3], f->v[srt[2].index][3], 0 };
    vec4i xmx = __builtin_shuffle(xs, hmask) - xs;
    vec4i ymy = __builtin_shuffle(ys, hmask) - ys;
    vec4f zmz = __builtin_shuffle(zs, hmask) - zs;

    const int orient = (xmx[0] * ymy[2]) - (ymy[0] * xmx[2]);
    float ma = (float)xmx[0] / ymy[0];
    float mb = (float)xmx[2] / ymy[2];
    float za = zmz[0] / ymy[0];
    float zb = zmz[2] / ymy[2];
    if (orient < 0) {
        swap(&ma, &mb, 4);
        swap(&za, &zb, 4);
    }

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
                if ( depthZ > height_map[padxSB] ) {

                    height_map[padxSB] = depthZ;
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
            if ( depthZ > height_map[padxSB] ) {

                height_map[padxSB] = depthZ;
            }
            xxs += 1.0;
        }
        yB++;
    }
}
/* Releases all members of the given inside Height_map pipeline lvl 1 Mesh. */
const static void releaseMeshHeightStepOne(MeshHeightStepOne *c) {
    free(c->v);
    free(c->n);
}
/* Releases all members of the given inside Height_map pipeline lvl 2 Mesh. */
const static void releaseMeshHeightStepTwo(MeshHeightStepTwo *c) {
    free(c->f);
}
/* Initializing Mesh a from Mesh b. */
const static void initMeshHeightStepOne(MeshHeightStepOne *a, Mesh *b) {
    size_t vsize = sizeof(vec4f) * b->v_indexes;
    size_t nsize = sizeof(vec4f) * b->n_indexes;

    a->v = malloc(vsize);
    memcpy(a->v, b->v, vsize);
    a->v_indexes = b->v_indexes;

    a->n = malloc(nsize);
    memcpy(a->n, b->n, nsize);
    a->n_indexes = b->n_indexes;

    a->cull = b->cull;
}





