#include "headers/gravity.h"

const void applyGravity(Scene *s) {
    /* Apply Gravitanional forces to Specific or all meshes. EXCEPT TERRAIN. */
    for (int i = 0; i < s->m_indexes; i++) {

        if ((s->m[i].type != Terrain) && (!s->m[i].grounded)) {

            s->m[i].falling_time += DeltaTime;
            const vec4f pull_point = { 0.f, -1.f, 0.f };
            const float velocity = 9.81f * s->m[i].falling_time;

            vec4f pivot = (pull_point * velocity);

            s->m[i].pivot += pivot;

            objectTerrainCollision(&s->m[Terrain_1], &s->m[i]);
            addMeshToQuad(&s->m[i]);
        }
    }
}


