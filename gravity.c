#include "headers/gravity.h"

/* Defined in main.c. */
extern TerrainInfo tf;
#include "headers/logging.h"
const int EnvironmentCollision(TerrainInfo *tf, Scene *s, Mesh *obj) {
    if (obj->quadIndex < 0) {
        fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", obj->quadIndex);
        return 0;
    }

    vec4f Q = obj->mvdir * obj->momentum;
    vec4f D = (obj->pivot + Q) - obj->pivot;

    obj->BB = getDimensionsLimits(obj->v, obj->v_indexes);

    float tnearx, tnearz, tfarx, tfarz;

    const int num_of_members = tf->quads[obj->quadIndex].mems_indexes;
    for (int i = 0; i < num_of_members; i++) {

        int inner_inx = tf->quads[obj->quadIndex].mems[i];

        if ( s->m[inner_inx].id != obj->id && s->m[inner_inx].type != Player ) {
            s->m[inner_inx].BB = getDimensionsLimits(s->m[inner_inx].v, s->m[inner_inx].v_indexes);

            float minx = s->m[inner_inx].BB.minX - (obj->pivot[0] - obj->BB.minX);
            float miny = s->m[inner_inx].BB.minY - (obj->pivot[1] - obj->BB.minY);
            float minz = s->m[inner_inx].BB.minZ - (obj->pivot[2] - obj->BB.minZ);
            float maxx = s->m[inner_inx].BB.maxX - (obj->pivot[0] - obj->BB.maxX);
            float maxy = s->m[inner_inx].BB.maxY - (obj->pivot[1] - obj->BB.maxY);
            float maxz = s->m[inner_inx].BB.maxZ - (obj->pivot[2] - obj->BB.maxZ);

            tnearx = (minx - obj->pivot[0]) / D[0];
            tnearz = (minz - obj->pivot[2]) / D[2];
            tfarx = (maxx - obj->pivot[0]) / D[0];
            tfarz = (maxz - obj->pivot[2]) / D[2];
            // printf("tnearx: %f    tnearz: %f    tfarx: %f    tfarz: %f\n", tnearx, tnearz, tfarx, tfarz);

            if (tnearx > tfarx) swap(&tnearx, &tfarx, 4);
            if (tnearz > tfarz) swap(&tnearz, &tfarz, 4);


            if (tnearx > tfarz || tnearz > tfarx) {
                // printf("Unable to Collide!\n");
                continue;
            }

            float t_near = tnearx > tnearz ? tnearx : tnearz;
            float t_far = tfarx < tfarz ? tfarx : tfarz;

            if (t_far < 0) { 
                // printf("Collision in negative direction!\n");
                continue;
            }

            vec4f normal = { 0 };
            if ( tnearx > tnearz ) {
                if ( D[0] < 0 )
                    normal[0] = 1;
                else     
                    normal[0] = -1;
            } else if ( tnearx < tnearz ) {
                if ( D[2] < 0 )
                    normal[2] = 1;
                else
                    normal[2] = -1;
            }

            if (t_near <= 1.f) {
                printf("Collision!\n");

                obj->momentum *= s->m[inner_inx].mass;

                s->m[inner_inx].mvdir = obj->mvdir;
                s->m[inner_inx].momentum = obj->momentum;

                // obj->mvdir = normal + obj->mvdir;
                float dot =  dot_product(normal, obj->mvdir);
                obj->mvdir = obj->mvdir - (dot * normal);

                Q[0] = fabsf(Q[0]);
                Q[1] = fabsf(Q[1]);
                Q[2] = fabsf(Q[2]);

                vec4f pivot = Q * (1.f - t_near) * normal;

                Mat4x4 trans = translationMatrix(pivot[0], pivot[1], pivot[2]);
                obj->v = setvecsarrayxm(obj->v, obj->v_indexes, trans);
                obj->n = setvecsarrayxm(obj->n, obj->n_indexes, trans);

                obj->pivot += pivot;
            }
        }
    }
}

const void applyForces(Scene *s) {
    Mat4x4 trans;
    for (int i = 0; i < s->m_indexes; i++) {
        if ( s->m[i].momentum >= 0 ) {

            s->m[i].momentum -= DeltaTime;
            if ( s->m[i].momentum <= 0 ) {
                s->m[i].momentum = s->m[i].roll = 0.f;
                continue;
            }

            EnvironmentCollision(&tf, s, &s->m[i]);

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



