#include "headers/collision_detection.h"

#include "headers/logging.h"
extern int DROPBALL;
extern float GravityTime;
const int checkCollision(Scene *s) {
    vec4f cache_0;
    for (int i = 0; i < s->m_indexes; i++) {
        if ( s->m[i].type != Terrain ) {
            logVec4f(s->m[i].pivot);
            // cache_0 = vecxm(s->m[i].pivot, heightWorld);

            float w = cache_0[3];
            cache_0 /= w;

            cache_0[0] = ((1 + cache_0[0]) * HALFW) + 0.5;
            cache_0[1] = ((1 + cache_0[1]) * HALFH) + 0.5;
            cache_0[3] = 1 / (w + s->m[i].scale + 1);

            const int padHM = (cache_0[1] * main_wa.width) + cache_0[0];
            // logVec4f(cache_0);
            if ( cache_0[3]  >= height_map[padHM] ) {
                printf("Collision detected : %f, %f\n",cache_0[3], height_map[padHM]);
                DROPBALL = 0;
                GravityTime = 0;
                // Mat4x4 trans = translationMatrix(s->m[i].pivot[0], height_map[padHM]* s->m[i].scale, s->m[i].pivot[2]);
                // s->m[i].v = setvecsarrayxm(s->m[i].v, s->m[i].v_indexes, trans);
                // s->m[i].pivot[1] = height_map[padHM]* s->m[i].scale;
                // logVec4f(s->m[i].pivot - (cache_0[3] * s->m[i].scale));
            }
        }
    }
}



