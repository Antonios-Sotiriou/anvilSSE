#include "headers/collision_detection.h"

#include "headers/logging.h"
#include "headers/vectors_math.h"
#include "headers/matrices.h"
#include "headers/general_functions.h"
extern int DROPBALL, HALFW, HALFH;
extern float collNPlane, collFPlane;
extern float GravityTime;
extern Mat4x4 perspMat, orthoMat;
Mat4x4 collMat;

const int checkTerrainCollision(Mesh *m, const float terrain_height) {
    vec4f cache_0;
    if ( m->type != Terrain ) {

    }
}
const int getPossibleColliders(Scene *s, Mesh *m) {
    vec4f up = { 0.f, -1.f, 0.f };
    vec4f u = cross_product(m->mvdir, up);
    vec4f v = cross_product(u, m->mvdir);

    Mat4x4 lk = lookat(m->pivot, u, v, m->mvdir);
    collMat = mxm(inverse_mat(lk), orthoMat);
 
    collideRangeCulling(s->m, s->m_indexes);
}
const void collideRangeCulling(Mesh *m, const int len) {
    vec4f *vec_arr;
    DimensionsLimits dm;

    for (int i = 0; i < len; i++) {
        vec_arr = vecsarrayxm(m[i].v, m[i].v_indexes, collMat);

        dm = getDimensionsLimits(vec_arr, m[i].v_indexes);

        if ( ((dm.minZ > collFPlane) || (dm.maxZ < collNPlane)) ||
             ((dm.minY > 1) || (dm.maxY < -1)) ||
             ((dm.minX > 1) || (dm.maxX < -1)) ) {

            free(vec_arr);
            m[i].visible = 0;
            continue;
        }
        free(vec_arr);
        m[i].visible = 1;
    }
}



