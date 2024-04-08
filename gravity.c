#include "headers/gravity.h"

/* Defined in main.c. */
extern TerrainInfo tf;
#include "headers/logging.h"
const int EnvironmentCollision(TerrainInfo *tf, Scene *s, Mesh *obj) {
    if (obj->quadIndex < 0) {
        fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", obj->quadIndex);
        return 0;
    }

    const int num_of_members = tf->quads[obj->quadIndex].mems_indexes;
    obj->BB = getDimensionsLimits(obj->v, obj->v_indexes);
    // printf("\nChecking collisions obj->id: %d --> ", obj->id);

    for (int i = 0; i < num_of_members; i++) {

        int inner_inx = tf->quads[obj->quadIndex].mems[i];

        if ( s->m[inner_inx].id != obj->id ) {
            s->m[inner_inx].BB = getDimensionsLimits(s->m[inner_inx].v, s->m[inner_inx].v_indexes);
            // printf("%d ", s->m[inner_inx].id);

            if (obj->BB.minZ > s->m[inner_inx].BB.minZ && obj->BB.minZ < s->m[inner_inx].BB.maxZ || 
                obj->BB.maxZ > s->m[inner_inx].BB.minZ && obj->BB.maxZ < s->m[inner_inx].BB.maxZ ||
                obj->BB.minZ < s->m[inner_inx].BB.minZ && obj->BB.maxZ > s->m[inner_inx].BB.maxZ) {

                if (obj->BB.minX > s->m[inner_inx].BB.minX && obj->BB.minX < s->m[inner_inx].BB.maxX || 
                    obj->BB.maxX > s->m[inner_inx].BB.minX && obj->BB.maxX < s->m[inner_inx].BB.maxX || 
                    obj->BB.minX < s->m[inner_inx].BB.minX && obj->BB.maxX > s->m[inner_inx].BB.maxX) {

                    // if (obj->momentum <= 0)
                    //     obj->momentum = 1;

                    // printf("\nCollision Detected ids %d, %d!", obj->id, s->m[inner_inx].id);
                    s->m[inner_inx].momentum = obj->momentum;
                    s->m[inner_inx].mvdir = obj->mvdir;
                    obj->momentum *= s->m[inner_inx].mass;

                    // obj->collide = 1;
                    // logVec4f(norm_vec((obj->pivot + obj->scale) - (s->m[inner_inx].pivot + s->m[inner_inx].scale)));
                    // obj->pivot += ((obj->pivot + obj->scale) - (s->m[inner_inx].pivot + s->m[inner_inx].scale)) * obj->mvdir; 
                    // printf("A: %f\n", (obj->pivot[0] + obj->scale) - (s->m[inner_inx].pivot[0] + s->m[inner_inx].scale));
                    // logVec4f(obj->pivot);
                    // logVec4f(s->m[inner_inx].pivot);
                    // printf("B: %f\n", (obj->pivot[2] + obj->scale) - (s->m[inner_inx].pivot[2] + s->m[inner_inx].scale));
                    // printf("A: %f    B: %f\n", obj->momentum, s->m[inner_inx].momentum);
                }
            }
        }
    }
    if (obj->collide) {
        // obj->collide = 0;
        return 1;
    }
    obj->collide = 0;
    return 0;
}

const void applyForces(Scene *s) {
    Mat4x4 trans;
    for (int i = 0; i < s->m_indexes; i++) {
        if ( s->m[i].momentum > 0 ) {

            // s->m[i].momentum -= DeltaTime;
            if ( s->m[i].momentum < 0 )
                s->m[i].momentum = s->m[i].roll = 0;

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
            // EnvironmentCollision(&tf, s, &s->m[i]);
        }
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



