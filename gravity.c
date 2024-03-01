#include "headers/gravity.h"

const void applyForces(Scene *s) {
    Mat4x4 trans;
    for (int i = 0; i < s->m_indexes; i++) {
        if (s->m[i].momentum > 0) {

            s->m[i].momentum -= DeltaTime;
            vec4f pivot = s->m[i].mvdir * s->m[i].momentum;
            vec4f axis = { 1.f, 0.f, 0.f };

            if (s->m[i].roll) {
                s->m[i].roll = s->m[i].momentum * 10;

                Quat xrot = rotationQuat(s->m[i].roll, axis);
                Mat4x4 m = MatfromQuat(xrot, s->m[i].pivot);
                trans = mxm(m, translationMatrix(pivot[0], pivot[1], pivot[2]));

                s->m[i].Q = multiplyQuats(s->m[i].Q, xrot);
            } else {
                trans = translationMatrix(pivot[0], pivot[1], pivot[2]);
            }

            s->m[i].v = setvecsarrayxm(s->m[i].v, s->m[i].v_indexes, trans);
            s->m[i].n = setvecsarrayxm(s->m[i].n, s->m[i].n_indexes, trans);

            s->m[i].pivot += pivot;
        } else
            s->m[i].momentum = s->m[i].roll = 0;
    }
}
const void applyGravity(Scene *s) {
    Mat4x4 trans;
    /* Apply Gravitanional forces to Specific or all meshes. EXCEPT TERRAIN. */
    for (int i = 0; i < s->m_indexes; i++) {
        if ( (s->m[i].type != Terrain) && (!s->m[i].grounded) || (s->m[i].momentum) ) {

            s->m[i].falling_time += DeltaTime;
            const vec4f pull_point = { 0.f, -1.f, 0.f };
            const float velocity = 9.81f * s->m[i].falling_time;

            vec4f pivot = (pull_point * velocity);

            trans = translationMatrix(pivot[0], pivot[1], pivot[2]);
            s->m[i].v = setvecsarrayxm(s->m[i].v, s->m[i].v_indexes, trans);
            s->m[i].n = setvecsarrayxm(s->m[i].n, s->m[i].n_indexes, trans);

            s->m[i].pivot += pivot;

            objectTerrainCollision(&s->m[Terrain_1], &s->m[i]);
            addMeshToQuad(&s->m[i]);
        }
    }
}



