#include "../headers/edge_pipeline.h"

const static int frustumCulling(vec4f v[], const int v_indexes);
const static MeshStepTwo assemblyfaces(MeshStepOne *m, unsigned int *indices, const int len);
const static void ppdiv(MeshStepTwo *m, const int len);
const static MeshStepTwo bfculling(const MeshStepTwo m, const int len);
const static int viewtoscreen(MeshStepTwo *m, const int len);
const static void rasterize(MeshStepTwo *m, Material *mtr);
const static void releaseMeshStepOne(MeshStepOne *c);
const static void releaseMeshStepTwo(MeshStepTwo *c);
const static void initMeshStepOne(MeshStepOne *a, Mesh *b);
const static vec4f add_one = { 1.f, 1.f, 0.f, 0.f };
const static vec4f add_half = { 0.5f, 0.5f, 0.f, 0.f };

/* Passes the scene Meshes throught the graphic pipeline. */
const void edgePipeline(void) {

    MeshStepOne cache_0 = { 0 };

    for (int i = 0; i < scene.m_indexes; i++) {

        initMeshStepOne(&cache_0, &scene.m[i]);

        cache_0.bbox.v = setvecsarrayxm(cache_0.bbox.v, cache_0.bbox.v_indexes, worldMat);

        if (frustumCulling(cache_0.bbox.v, cache_0.bbox.v_indexes)) {

            Mat4x4 sclMatrix, trMatrix, enWorldMatrix;

            vec4f pos = { 0 };
            Mat4x4 mfQ = MatfromQuat(scene.m[i].Q, pos);
            sclMatrix = mxm(mfQ, scaleMatrix(scene.m[i].scale));
            trMatrix = translationMatrix(scene.m[i].pivot[0], scene.m[i].pivot[1], scene.m[i].pivot[2]);
            enWorldMatrix = mxm(sclMatrix, trMatrix);

            Mat4x4 vecsMat = mxm(enWorldMatrix, worldMat);

            cache_0.v = setvecsarrayxm(cache_0.v, cache_0.v_indexes, vecsMat);

            /* Assembly and create the faces from the mesh vertices, normals and texture arrays, through the indexes. */
            MeshStepTwo cache_1 = assemblyfaces(&cache_0, scene.m[i].f, scene.m[i].f_indexes);
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

            rasterize(&cache_1, &scene.m[i].material);
            releaseMeshStepTwo(&cache_1);
        } else {
            releaseMeshStepOne(&cache_0);
        }
    }
}
/* Cull Mesh to view frustum. viewProj: (1 for Prespective and 0 for orthographic Projection). Thats for the perspective divide usefull.viewMat the matrix of the point of view. */
const static int frustumCulling(vec4f v[], const int v_indexes) {
    /* Thats a fix for unitialized meshes that cannot become visible due to no vectors initialization. That will be corrected with bounding boxes. */
    if (!v_indexes) {
        return 1;
    }

    vec4f *vec_arr = malloc(v_indexes * 16);
    memcpy(vec_arr, v, v_indexes * 16);

    for (int j = 0; j < v_indexes; j++) {
        /* We save Clipp space z for frustum culling because Near and far planes are defined in this Space. */
        float z = vec_arr[j][2];

        if (vec_arr[j][3] > 0) {
            vec_arr[j] /= vec_arr[j][3];
            vec_arr[j][2] = z;
        }
    }

    DimensionsLimits dm = getDimensionsLimits(vec_arr, v_indexes);

    if ( ((dm.min[2] > FPlane) || (dm.max[2] < NPlane)) ||
            ((dm.min[1] > 1.f) || (dm.max[1] < -1.f)) ||
            ((dm.min[0] > 1.f) || (dm.max[0] < -1.f)) ) {

        free(vec_arr);
        return 0;
    }

    free(vec_arr);
    return 1;
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
        index++;
    }

    r.cull = m->cull;
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
    // Mat4x4 ssm = screenSpaceMatrix();
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 3; j++) {
            m->f[i].v[j] = ((add_one + m->f[i].v[j]) * __builtin_convertvector(half_screen, vec4f)) + add_half;
        }
    }

    /* Viewport clipping. */
    vec4f plane_up_p = { 0.0f, 0.f, 0.0f },
        plane_up_n = { 0.0f, 1.0f, 0.0f };
    *m = clipp(m, plane_up_p, plane_up_n);
    if(!m->f_indexes)
        return 0;

    vec4f plane_down_p = { 0.0f, main_wa.height - 1, 0.0f },
        plane_down_n = { 0.0f, -1.0f, 0.0f };
    *m = clipp(m, plane_down_p, plane_down_n);
    if(!m->f_indexes)
        return 0;

    vec4f plane_left_p = { 0.f, 0.0f, 0.0f },
        plane_left_n = { 1.0f, 0.0f, 0.0f };
    *m = clipp(m, plane_left_p, plane_left_n);
    if(!m->f_indexes)
        return 0;

    vec4f plane_right_p = { main_wa.width - 1, 0.0f, 0.0f },
        plane_right_n = { -1.0f, 0.0f, 0.0f };
    *m = clipp(m, plane_right_p, plane_right_n);
    if(!m->f_indexes)
        return 0;

    return 1;
}
/* Rasterize given Mesh by passing them to the appropriate function. */
const static void rasterize(MeshStepTwo *m, Material *mtr) {
    point_frame_buffer = frame_buffer;

    edgeMesh(m, mtr->basecolor);
}
/* Releases all members of the given inside graphic pipeline lvl 1 Mesh. */
const static void releaseMeshStepOne(MeshStepOne *c) {
    free(c->v);
    free(c->n);
    free(c->t);
    free(c->bbox.v);
}
/* Releases all members of the given inside graphic pipeline lvl 2 Mesh. */
const static void releaseMeshStepTwo(MeshStepTwo *c) {
    free(c->f);
}
/* Initializing Mesh a from Mesh b. */
const static void initMeshStepOne(MeshStepOne *a, Mesh *b) {
    size_t vsize = sizeof(vec4f) * b->v_indexes;
    size_t nsize = sizeof(vec4f) * b->n_indexes;
    size_t tsize = sizeof(vec2f) * b->t_indexes;
    size_t bbox_vsize = sizeof(vec4f) * b->bbox.v_indexes;

    a->v = malloc(vsize);
    memcpy(a->v, b->v, vsize);
    a->v_indexes = b->v_indexes;

    a->n = malloc(nsize);
    memcpy(a->n, b->n, nsize);
    a->n_indexes = b->n_indexes;

    a->t = malloc(tsize);
    memcpy(a->t, b->t, tsize);
    a->t_indexes = b->t_indexes;

    a->bbox.v = malloc(bbox_vsize);
    memcpy(a->bbox.v, b->bbox.v, bbox_vsize);
    a->bbox.v_indexes = b->bbox.v_indexes;

    a->cull = b->cull;
}


