#include "headers/collision_detection.h"

#include "headers/logging.h"
#include "headers/vectors_math.h"
#include "headers/matrices.h"
#include "headers/general_functions.h"
#include "headers/height_pipeline.h"
extern int DROPBALL, HALFW, HALFH;
extern float collNPlane, collFPlane;
extern float GravityTime;
extern Mat4x4 perspMat, orthoMat;
Mat4x4 collMat;
extern Scene scene;

const int getPossibleColliders(Scene *s) {
    for (int i = 0; i < s->m_indexes; i++) {
        if (s->m[i].rahm) {
            vec4f up = { 0.f, -1.f, 0.f };
            vec4f u = cross_product(s->m[i].mvdir, up);
            vec4f v = cross_product(u, s->m[i].mvdir);

            Mat4x4 lk = lookat(s->m[i].pivot, u, v, s->m[i].mvdir);
            collMat = mxm(inverse_mat(lk), orthoMat);
        
            collideRangeCulling(s->m, s->m_indexes, &s->m[i]);
            s->m[i].rahm = 0;
        }
    }
}
const void collideRangeCulling(Mesh *m, const int len, Mesh *cl) {
    vec4f *vec_arr;
    DimensionsLimits dm;

    for (int i = 0; i < len; i++) {
        if ( (m[i].id != cl->id) && m[i].type != Terrain ) {
            vec_arr = vecsarrayxm(m[i].v, m[i].v_indexes, collMat);

            dm = getDimensionsLimits(vec_arr, m[i].v_indexes);

            if ( ((dm.minZ > m[i].scale ) || (dm.minZ < 0)) ||
                ((dm.minY > 0.1) || (dm.maxY < -0.1)) ||
                ((dm.minX > 0.1) || (dm.maxX < -0.1)) ) {

                free(vec_arr);
                m[i].rahm = 0;
                continue;
            }
            free(vec_arr);
            m[i].rahm = 1;
            m[i].mvdir = norm_vec(m[i].pivot * cl->mvdir);
            Mat4x4 tr = translationMatrix(m[i].mvdir[0], m[i].mvdir[1], m[i].mvdir[2]);
            m[i].v = setvecsarrayxm(m[i].v, m[i].v_indexes, tr);
            m[i].pivot += m[i].mvdir;
            if (m[i].grounded) {
                const float height = getTerrainHeight(&scene, &m[i].pivot);
                float height_diff = height - (m[i].pivot[1] - m[i].scale);
                Mat4x4 dr = translationMatrix(0, height_diff, 0);
                m[i].v = setvecsarrayxm(m[i].v, m[i].v_indexes, dr);
                m[i].pivot[1] += height_diff;
            }
        }
    }
}



