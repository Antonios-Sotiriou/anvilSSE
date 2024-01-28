#include "headers/gravity.h"

Mat4x4 trans;

#include "headers/logging.h"
#include "headers/height_pipeline.h"
extern float DeltaTime, GravityTime;

const void applyGravity(Scene *s, const float FallTime) {
    /* Apply Gravitanional forces to Specific or all meshes. EXCEPT TERRAIN. */
    for (int i = 0; i < s->m_indexes; i++) {
        if ((s->m[i].type != Terrain) && (!s->m[i].grounded) && s->m[i].type != MovingObject) {
            GravityTime += DeltaTime;
            const vec4f pull_point = { 0.f, -1.f, 0.f };
            const float velocity = 9.81f * GravityTime;
            // printf("fraction: %f\n", velocity);
            // printf("BEFORE: ");
            // logVec4f(s->m[i].pivot);
            vec4f pivot = (pull_point * velocity);
            // printf("AFTER : ");

            trans = translationMatrix(pivot[0], pivot[1], pivot[2]);
            s->m[i].v = setvecsarrayxm(s->m[i].v, s->m[i].v_indexes, trans);

            s->m[i].pivot += pivot;

            // printf("inside pivot: ");
            // logVec4f(s->m[i].pivot);

            // logMatrix(trans);

            // printf("mesh name: %s\n", s->m[i].name);
        }
    }
    const float height = getTerrainHeight(s, &s->m[4].pivot);
    // printf("height: %f\n", height);
    float height_diff = height - (s->m[4].pivot[1] - s->m[4].scale);
    if (height_diff < 0 && !s->m[4].grounded) {
        // Mat4x4 dr = translationMatrix(0, height_diff, 0);
        // s->m[4].v = setvecsarrayxm(s->m[4].v, s->m[4].v_indexes, dr);
        // s->m[4].pivot[1] += height_diff;
        // s->m[4].grounded = 0;
        // printf("Clause: 1\n");
    } else if (height_diff >= 0) {
        // Mat4x4 dr = translationMatrix(0, height_diff, 0);
        // s->m[4].v = setvecsarrayxm(s->m[4].v, s->m[4].v_indexes, dr);
        // s->m[4].pivot[1] += height_diff;
        s->m[4].grounded = 1;
        GravityTime = 0;
        // printf("Clause: 2\n");
    }
    if (s->m[4].grounded) {
        Mat4x4 dr = translationMatrix(0, height_diff, 0);
        s->m[4].v = setvecsarrayxm(s->m[4].v, s->m[4].v_indexes, dr);
        s->m[4].pivot[1] += height_diff;
        // printf("Clause: 3\n");
        // logVec4f(s->m[4].pivot);
    }
    // printf("outside pivot: ");
    // logVec4f(s->m[4].pivot);
    // printf("height diff: %f\n", height_diff);

    // if (height_diff < 4.842860)
    //     exit(0);
    printf("grounded: %d\n", s->m[4].grounded);
}


