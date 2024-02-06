#include "headers/gravity.h"

extern float DeltaTime;
#include "headers/logging.h"
const void applyGravity(Scene *s, const float FallTime) {
    Mat4x4 trans;
    /* Apply Gravitanional forces to Specific or all meshes. EXCEPT TERRAIN. */
    for (int i = 0; i < s->m_indexes; i++) {
        if ((s->m[i].type != Terrain) && (!s->m[i].grounded)) {

            s->m[i].falling_time += DeltaTime;
            const vec4f pull_point = { 0.f, -1.f, 0.f };
            const float velocity = 9.81f * s->m[i].falling_time;

            vec4f pivot = (pull_point * velocity);

            trans = translationMatrix(pivot[0], pivot[1], pivot[2]);
            s->m[i].v = setvecsarrayxm(s->m[i].v, s->m[i].v_indexes, trans);

            s->m[i].pivot += pivot;

            const float height = getTerrainHeight(&s->m[Terrain_1], s->m[i].pivot, &s->m[i]);
            addMeshToQuad(&s->m[i]);
            float height_diff = height - (s->m[i].pivot[1] - s->m[i].scale);
            if (height_diff >= 0) {
                s->m[i].grounded = 1;
                s->m[i].falling_time = 0;
            }
            if (s->m[i].grounded) {
                Mat4x4 dr = translationMatrix(0, height_diff, 0);
                s->m[i].v = setvecsarrayxm(s->m[i].v, s->m[i].v_indexes, dr);
                s->m[i].pivot[1] += height_diff;
            }
        }
    }

    const float height = getTerrainHeight(&s->m[Terrain_1], s->m[Player_1].pivot, &s->m[Player_1]);
    float height_diff = height - (s->m[1].pivot[Player_1] - s->m[Player_1].scale);
    if (height_diff >= 0) {
        s->m[Player_1].grounded = 1;
        s->m[Player_1].falling_time = 0;
    }
    if (s->m[1].grounded) {
        Mat4x4 dr = translationMatrix(0, height_diff, 0);
        s->m[Player_1].v = setvecsarrayxm(s->m[Player_1].v, s->m[Player_1].v_indexes, dr);
        s->m[Player_1].pivot[1] += height_diff;
    }
}


