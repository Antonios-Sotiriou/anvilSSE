#include "headers/gravity.h"

Mat4x4 trans;

#include "headers/logging.h"

const void applyGravity(Scene *s, const float FallTime) {
    /* Apply Gravitenional forces to Specific or all meshes. EXCEPT TERRAIN. */
    for (int i = 0; i < s->m_indexes; i++) {
        if (s->m[i].type != Terrain) {
            
            const vec4f pull_point = { 0.f, -1.f, 0.f };
            const float velocity = 9.81f * FallTime;
            // printf("fraction: %f\n", velocity);
            // printf("BEFORE: ");
            // logVec4f(s->m[i].pivot);
            vec4f pivot = (pull_point * velocity);
            // printf("AFTER : ");

            trans = translationMatrix(pivot[0], pivot[1], pivot[2]);
            s->m[i].v = setvecsarrayxm(s->m[i].v, s->m[i].v_indexes, trans);

            s->m[i].pivot += pivot;
            logVec4f(s->m[i].pivot);

            // logMatrix(trans);

            // printf("mesh name: %s\n", s->m[i].name);
        }
    }
}


