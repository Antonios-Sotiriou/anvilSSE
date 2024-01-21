#include "headers/collision_detection.h"

#include "headers/logging.h"
extern int DROPBALL;
extern float GravityTime;
const int checkTerrainCollision(Mesh *m, const float terrain_height) {
    vec4f cache_0;
    if ( m->type != Terrain ) {

        // logVec4f(cache_0);
        DROPBALL = 0;
        GravityTime = 0;
        // Mat4x4 trans = translationMatrix(s->m[i].pivot[0], height_map[padHM]* s->m[i].scale, s->m[i].pivot[2]);
        // s->m[i].v = setvecsarrayxm(s->m[i].v, s->m[i].v_indexes, trans);
        // s->m[i].pivot[1] = height_map[padHM]* s->m[i].scale;
        // logVec4f(s->m[i].pivot - (cache_0[3] * s->m[i].scale));
    }
}



