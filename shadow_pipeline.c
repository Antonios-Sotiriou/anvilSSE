#include "headers/shadow_pipeline.h"

const static void enShadowMesh(Mesh *m, const Mat4x4 lightMat);
const static MeshShadowStepTwo assemblyfacesShadow(MeshShadowStepOne *m, unsigned int *indices, const int len);
const static MeshShadowStepTwo shadowculling(const MeshShadowStepTwo c, const int len);
const static float shadow_winding(const Shadowface f);
const static int shadowtoscreen(MeshShadowStepTwo *m, const int len);
const static void createShadowmap(MeshShadowStepTwo *m, const unsigned int sm_index);
const static void shadowface(Shadowface *f, const Srt srt[], const unsigned int sm_index);
const static void releaseMeshShadowStepOne(MeshShadowStepOne *c);
const static void releaseMeshShadowStepTwo(MeshShadowStepTwo *c);
const static void initMeshShadowStepOne(MeshShadowStepOne *a, Mesh *b);
const static vec4i smask = { 1, 2, 0, 3 };
static Mat4x4 lview;

/* ################################################### CASCADE SHADOW MAPPING START   ################################################ */
/* Returns a vec4f array with the values of the 8 vectors that form the View Frustum in View Space.np and fp are the near and far planes( can be Choosen accordingly ). */
vec4f *worldSpaceFrustum(const float np, const float fp) {
    vec4f *va = malloc(128);
    const float fovRadius = (1.f / tanf(45.f * 0.5f / 180.0f * 3.14159f));

    const vec4f nearcenter = (eye[0] + eye[3]) * np;
    const vec4f farcenter = (eye[0] + eye[3]) * fp;

    const float nearHeight = tan(fovRadius) * np;
    const float farHeight = tan(fovRadius) * fp;
    const float nearWidth = nearHeight * ASPECTRATIO;
    const float farWidth = farHeight * ASPECTRATIO;

    const vec4f yxnh = eye[2] * nearHeight;
    const vec4f yxnw = eye[1] * nearWidth;

    const vec4f yxfh = eye[2] * farHeight;
    const vec4f yxfw = eye[1] * farWidth;

    va[2] = nearcenter + yxnh + yxnw;
    va[3] = nearcenter - yxnh + yxnw;
    va[6] = nearcenter + yxnh - yxnw;
    va[7] = nearcenter - yxnh - yxnw;

    va[0] = farcenter + yxfh + yxfw;
    va[1] = farcenter - yxfh + yxfw;
    va[4] = farcenter + yxfh - yxfw;
    va[5] = farcenter - yxfh - yxfw;

    return va;
}
const Mat4x4 createOrthoMatrixFromLimits(const DimensionsLimits dl) {
    return orthographicMatrix(dl.minX, dl.maxX, dl.minY, dl.maxY, dl.minZ, dl.maxZ);
}
const void createCascadeShadowMatrices(const unsigned int num_of_cascades) {
    DimensionsLimits dl;
    vec4f *fr[3] = {
        worldSpaceFrustum(NPlane, 100.f),
        worldSpaceFrustum(NPlane, 250.f),
        worldSpaceFrustum(NPlane, 500.f)
    };
    Mat4x4 lm[3] = {
        lookat(eye[0] + (eye[3] * (float)SMA), sunlight.u, sunlight.v, sunlight.n),//0.f
        lookat(eye[0] + (eye[3] * (float)SMB), sunlight.u, sunlight.v, sunlight.n),//158.f
        lookat(eye[0] + (eye[3] * (float)SMC), sunlight.u, sunlight.v, sunlight.n) //316.f
    };

    for (int i = 0; i < num_of_cascades; i++) {
        lm[i].m[3][1] = sunlight.pos[1];
        lview = inverse_mat(lm[i]);
        /* Transform view frustum to Space. */
        fr[i] = setvecsarrayxm(fr[i], 8, viewMat);

        dl = getDimensionsLimits(fr[i], 8);
        free(fr[i]);

        ortholightMat[i] = mxm(lview, createOrthoMatrixFromLimits(dl));
    }
}
#include "headers/logging.h"
/* ################################################### CASCADE SHADOW MAPPING FINISH ################################################ */
const void shadowPipeline(Scene *s, const unsigned int sm_index) {
    Mesh cache_v = { 0 };
    MeshShadowStepOne cache_0 = { 0 };

    for (int i = 0; i < s->m_indexes; i++) {
        initMesh(&cache_v, &s->m[i]);

        cache_v.v = setvecsarrayxm(cache_v.v, cache_v.v_indexes, ortholightMat[sm_index]);
        if (frustumCulling(&cache_v)) {

            initMeshShadowStepOne(&cache_0, &cache_v);

            MeshShadowStepTwo cache_1 = assemblyfacesShadow(&cache_0, cache_v.f, cache_v.f_indexes);
            releaseMesh(&cache_v);
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

            /* Applying Backface culling before we proceed to full frustum shadowclipping. */
            if (cache_1.cull)
                cache_1 = shadowculling(cache_1, cache_1.f_indexes);
            if (!cache_1.f_indexes) {
                releaseMeshShadowStepTwo(&cache_1);
                continue;
            }

            /* Sending to translation from NDC to Screen Coordinates. */
            if (!shadowtoscreen(&cache_1, cache_1.f_indexes)) {
                releaseMeshShadowStepTwo(&cache_1);
                continue;
            }

            createShadowmap(&cache_1, sm_index);
            releaseMeshShadowStepTwo(&cache_1);
        } else
            releaseMesh(&cache_v);
    }
}
const static void enShadowMesh(Mesh *m, const Mat4x4 lightMat) {
    Mat4x4 sclMatrix, trMatrix, vectorsMatrix;

    sclMatrix = scaleMatrix(m->scale);
    trMatrix = translationMatrix(m->pivot[0], m->pivot[1], m->pivot[2]);
    vectorsMatrix = mxm(mxm(sclMatrix, trMatrix), lightMat);

    m->v = setvecsarrayxm(m->v, m->v_indexes, vectorsMatrix);
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
/* Identifies if the Vectors are in clockwise order < CW > or not < CCW >. */
const float shadow_winding(const Shadowface f) {
    vec4f xs = { f.v[0][0], f.v[1][0], f.v[2][0], 0.0f };
    vec4f ys = { f.v[0][1], f.v[1][1], f.v[2][1], 0.0f };

    vec4f r = xs * __builtin_shuffle(ys, smask) - ys * __builtin_shuffle(xs, smask);
    return r[0] + r[1] + r[2];
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
const static void createShadowmap(MeshShadowStepTwo *m, const unsigned int sm_index) {
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
const static void shadowface(Shadowface *f, const Srt srt[], const unsigned int sm_index) {
    vec4i xs = { f->v[srt[0].index][0], f->v[srt[1].index][0], f->v[srt[2].index][0], 0 };
    vec4i ys = { f->v[srt[0].index][1], f->v[srt[1].index][1], f->v[srt[2].index][1], 0 };
    vec4f zs = { f->v[srt[0].index][2], f->v[srt[1].index][2], f->v[srt[2].index][2], 0 };
    vec4i xmx = __builtin_shuffle(xs, smask) - xs;
    vec4i ymy = __builtin_shuffle(ys, smask) - ys;
    vec4f zmz = __builtin_shuffle(zs, smask) - zs;

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
    unsigned int sm_index;          //88                //88              //246
    sm_index = (frag[2] <= STA) ? 0 : (frag[2] > STB && frag[2] <= STC) ? 1 : (frag[2] > STC && frag[2] <= 1000) ? 2 : 3;
    if (sm_index > 2)
        return 0;

<<<<<<< HEAD
    // float bias[3] = { 0.0007, 0.003, 0.0076 };
=======
    float bias[3] = { 0.0007, 0.003, 0.0076 };
>>>>>>> collisionDetection
    // // bias 0 = 0.0007,    bias 1 = 0.003,    bias 2 = 0.0076;
    // shadow_bias = sm_index < 2 ? bias[sm_index] : shadow_bias;

    /* Transform to Model space coordinates. */ /* Transform to Light space coordinates. */
    frag[3] = 1.f;
    frag = vecxm(frag, mxm(*point_mat, ortholightMat[sm_index]));

    /* Transform to Screen space coordinates. */
    frag[0] = (1.0 + frag[0]) * (main_wa.width >> 1);
    frag[1] = (1.0 + frag[1]) * (main_wa.height >> 1);
    frag[2] = 1.f / frag[2];

    float shadow = 0.0;
    for (int v = -1; v <= 1; v++) {
        for (int u = -1; u <= 1; u++) {

            if ( (frag[0] > 0 && frag[0] < main_wa.width) && (frag[1] > 0 && frag[1] < main_wa.height) )
                frag[0] += u, frag[1] += v;
            else
                return 0;


            float pcfDepth = shadow_buffer[sm_index][((int)frag[1] * main_wa.width) + (int)frag[0]];
<<<<<<< HEAD
            shadow += frag[2] + shadow_bias < pcfDepth ? 1.f : 0.f;
=======
            shadow += frag[2] + bias[sm_index] < pcfDepth ? 1.f : 0.f;
>>>>>>> collisionDetection
        }
    }

    return shadow / 9.f;
}
/* Releases all members of the given inside Shadow pipeline lvl 1 Mesh. */
const static void releaseMeshShadowStepOne(MeshShadowStepOne *c) {
    free(c->v);
}
/* Releases all members of the given inside Shadow pipeline lvl 2 Mesh. */
const static void releaseMeshShadowStepTwo(MeshShadowStepTwo *c) {
    free(c->f);
}
/* Initializing a MeshShadowStepOne a from Mesh b. */
const static void initMeshShadowStepOne(MeshShadowStepOne *a, Mesh *b) {
    size_t vsize = sizeof(vec4f) * b->v_indexes;

    a->v = malloc(vsize);
    memcpy(a->v, b->v, vsize);
    a->v_indexes = b->v_indexes;

    a->cull = b->cull;
}


