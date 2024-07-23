#include "../headers/grafik_pipeline.h"

const static int frustumCulling(vec4f v[], const int v_indexes, const int thread_id);
const static void ppdiv(Mesh *m, const int len);
const static Mesh bfculling(const Mesh m, const int len);
const static int viewtoscreen(Mesh *m, const int len, const int tile_index);
const static void rasterize(Mesh *m, Material *mtr);

const static vec4f add_one = { 1.f, 1.f, 0.f, 0.f };
const static vec4f add_half = { 0.5f, 0.5f, 0.f, 0.f };

/* Passes the scene Meshes throught the graphic pipeline. */
void *grafikPipeline(void *args) {
    int thread_id = *(int*)args;
    point_tiles = &tiles[thread_id];
    Mesh cache_0 = { 0 };

    for (int i = 0; i < scene.m_indexes; i++) {

        initMesh(&cache_0, &scene.m[i]);

        setvecsarrayxm(cache_0.bbox.v, cache_0.bbox.v_indexes, worldMat);

        if (frustumCulling(cache_0.bbox.v, cache_0.bbox.v_indexes, thread_id)) {

            Mat4x4 sclMatrix, trMatrix, enWorldMatrix;

            vec4f pos = { 0 };
            Mat4x4 mfQ = MatfromQuat(cache_0.Q, pos);
            sclMatrix = mxm(mfQ, scaleMatrix(cache_0.scale));
            trMatrix = translationMatrix(cache_0.cd.v[P][0], cache_0.cd.v[P][1], cache_0.cd.v[P][2]);
            enWorldMatrix = mxm(sclMatrix, trMatrix);

            Mat4x4 vecsMat = mxm(enWorldMatrix, worldMat);

            cache_0.f = compinedFaceNormals(cache_0.f, cache_0.f_indexes, vecsMat, enWorldMatrix);

            /* Clipping against near Plane in View Space. */
            vec4f plane_near_p = { 0.0f, 0.0f, NPlane },
                    plane_near_n = { 0.0f, 0.0f, 1.0f };
            cache_0 = clipp(&cache_0, plane_near_p, plane_near_n);
            if (!cache_0.f_indexes) {
                releaseMesh(&cache_0);
                continue;
            }

            /* Clipping against far Plane in View Space. */
            vec4f plane_far_p = { 0.0f, 0.0f, FPlane},
                plane_far_n = { 0.0f, 0.0f, -1.0f };
            cache_0 = clipp(&cache_0, plane_far_p, plane_far_n);
            if (!cache_0.f_indexes) {
                releaseMesh(&cache_0);
                continue;
            }

            /* Applying perspective division. */
            if (!PROJECTIONVIEW)
                ppdiv(&cache_0, cache_0.f_indexes);

            /* Applying Backface culling before we proceed to Screen Space transformation and view Port clipping. */
            if (cache_0.cull)
                cache_0 = bfculling(cache_0, cache_0.f_indexes);
            if (!cache_0.f_indexes) {
                releaseMesh(&cache_0);
                continue;
            }

            /* Sending to translation from NDC to Screen Coordinates. */
            if (!viewtoscreen(&cache_0, cache_0.f_indexes, thread_id)) {
                releaseMesh(&cache_0);
                continue;
            }

            rasterize(&cache_0, &scene.m[i].material);
            releaseMesh(&cache_0);
        } else {
            releaseMesh(&cache_0);
        }
    }
}
/* Cull Mesh to view frustum. viewProj: (1 for Prespective and 0 for orthographic Projection). Thats for the perspective divide usefull.viewMat the matrix of the point of view. */
const static int frustumCulling(vec4f v[], const int v_indexes, const int thread_id) {
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

    const float minY = ((float)tiles[thread_id].start_height / half_screen[1]) - 1.f;
    const float maxY = ((float)tiles[thread_id].end_height / half_screen[1]) - 1.f;

    if ( ((dm.min[2] > FPlane) || (dm.max[2] < NPlane)) ||
            ((dm.min[1] > maxY) || (dm.max[1] < minY)) ||
            ((dm.min[0] > 1.f) || (dm.max[0] < -1.f)) ) {

        free(vec_arr);
        return 0;
    }

    free(vec_arr);
    return 1;
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
const static int viewtoscreen(Mesh *m, const int len, const int tile_index) {
    // Mat4x4 ssm = screenSpaceMatrix(1000, 1000);
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 3; j++) {

            // printf("before  ");
            // logVec4f(vecxm(m->f[i].v[j] + add_one, ssm));

            m->f[i].vt[j] /= m->f[i].v[j][3];

            m->f[i].v[j] = ((add_one + m->f[i].v[j]) * __builtin_convertvector(half_screen, vec4f));

            // m->f[i].v[j][0] = ((1 + m->f[i].v[j][0]) * half_screen[0]) + 0.5f; /* adding 0.5 at this point so we can convert to integer at drawing stage. */
            // m->f[i].v[j][1] = ((1 + m->f[i].v[j][1]) * half_screen[1]) + 0.5f; /* adding 0.5 at this point so we can convert to integer at drawing stage. */
            // m->f[i].v[j][2] = 1.f / m->f[i].v[j][2];
            m->f[i].v[j][3] = 1.f / m->f[i].v[j][3];

            // printf("After   ");
            // logVec4f(m->f[i].v[j]);
            // printf("\n");
        }
    }

    /* Viewport clipping. */
    int clipp_bias = tile_index == 0 ? 0 : 1;
    int clipp_height = tile_index == 7 ? 1 : 0;
    vec4f plane_up_p = { 0.0f, tiles[tile_index].start_height - clipp_bias, 0.0f },
        plane_up_n = { 0.0f, 1.0f, 0.0f };
    *m = clipp(m, plane_up_p, plane_up_n);
    if(!m->f_indexes)
        return 0;

    vec4f plane_down_p = { 0.0f, tiles[tile_index].end_height - clipp_height, 0.0f },
        plane_down_n = { 0.0f, -1.0f, 0.0f };
    *m = clipp(m, plane_down_p, plane_down_n);
    if(!m->f_indexes)
        return 0;

    vec4f plane_left_p = { tiles[tile_index].start_width, 0.0f, 0.0f },
        plane_left_n = { 1.0f, 0.0f, 0.0f };
    *m = clipp(m, plane_left_p, plane_left_n);
    if(!m->f_indexes)
        return 0;

    vec4f plane_right_p = { tiles[tile_index].end_width, 0.0f, 0.0f },
        plane_right_n = { -1.0f, 0.0f, 0.0f };
    *m = clipp(m, plane_right_p, plane_right_n);
    if(!m->f_indexes)
        return 0;

    return 1;
}
/* Rasterize given Mesh by passing them to the appropriate function. */
const static void rasterize(Mesh *m, Material *mtr) {
    point_frame_buffer = frame_buffer;
    point_depth_buffer = main_depth_buffer;
    point_attrib = &main_wa;
    point_mat = &lookAt;

    if (DEBUG == 2)
        fillMesh(m, mtr);
    else
        texMesh(m, mtr);
}



