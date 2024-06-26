#include "headers/gravity.h"

/* Defined in main.c. */
extern TerrainInfo tf;
extern float movScalar;
#include "headers/logging.h"

const int EnvironmentCollision(TerrainInfo *tf, Scene *s, Mesh *obj) {
    if (obj->quadIndex < 0) {
        fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", obj->quadIndex);
        return 0;
    }

    vec4f Q = obj->mvdir * obj->momentum;
    vec4f D = (obj->pivot + Q) - obj->pivot;

    obj->BB = getDimensionsLimits(obj->v, obj->v_indexes);

    float tnearx, tneary, tnearz, tfarx, tfary, tfarz;

    const int num_of_members = tf->quads[obj->quadIndex].mems_indexes;
    for (int i = 0; i < num_of_members; i++) {

        int inner_inx = tf->quads[obj->quadIndex].mems[i];

        if ( s->m[inner_inx].id != obj->id ) {
            s->m[inner_inx].BB = getDimensionsLimits(s->m[inner_inx].v, s->m[inner_inx].v_indexes);

            float minx = s->m[inner_inx].BB.minX - (obj->pivot[0] - obj->BB.minX);
            float miny = s->m[inner_inx].BB.minY - (obj->pivot[1] - obj->BB.minY);
            float minz = s->m[inner_inx].BB.minZ - (obj->pivot[2] - obj->BB.minZ);
            float maxx = s->m[inner_inx].BB.maxX - (obj->pivot[0] - obj->BB.maxX);
            float maxy = s->m[inner_inx].BB.maxY - (obj->pivot[1] - obj->BB.maxY);
            float maxz = s->m[inner_inx].BB.maxZ - (obj->pivot[2] - obj->BB.maxZ);

            tnearx = (minx - obj->pivot[0]) / D[0];
            tneary = (miny - obj->pivot[1]) / D[1];
            tnearz = (minz - obj->pivot[2]) / D[2];
            tfarx = (maxx - obj->pivot[0]) / D[0];
            tfary = (maxy - obj->pivot[1]) / D[1];
            tfarz = (maxz - obj->pivot[2]) / D[2];
            // printf("x: %f    y: %f    z: %f  ", tnearx, tneary, tnearz);
            // printf("x: %f    y: %f    z: %f\n", tfarx, tfary, tfarz);

            if (tnearx > tfarx) swap(&tnearx, &tfarx, 4);
            if (tneary > tfary) swap(&tneary, &tfary, 4);
            if (tnearz > tfarz) swap(&tnearz, &tfarz, 4);


            if (tnearx > tfarz || tnearz > tfarx) {
                // printf("Unable to Collide!\n");
                continue;
            }

            float t_near = tnearx > tnearz ? tnearx : tnearz;
            float t_far = tfarx < tfarz ? tfarx : tfarz;

            /* ##################### Y ############################ */
            if (t_near > tfary || tneary > t_far) {
                // printf("Unable to Collide!\n");
                continue;
            }

            if (tneary > t_near)
                t_near = tneary;
            if (tfary < t_far)
                t_far = tfary;
            /* ##################### Y ############################ */

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
            } else {
                if ( D[1] < 0)
                    normal[1] = 1.f;
                else
                    normal[1] = -1.f;
            }
            vec4f pivot = { 0 };
            if ( t_near <= 1.f ) {
                printf("Collision 2\n");
                // printf("x: %f    y: %f    z: %f  ", tnearx, tneary, tnearz);
                // printf("x: %f    y: %f    z: %f\n", tfarx, tfary, tfarz);

                // obj->momentum = 0;
                // s->m[inner_inx].mvdir = obj->mvdir;
                // s->m[inner_inx].momentum = obj->momentum;
                if (normal[1] == 1.f) {
                    obj->overlap = 1;
                }

                Q[0] = fabsf(Q[0]);
                Q[1] = fabsf(Q[1]);
                Q[2] = fabsf(Q[2]);

                pivot = obj->mvdir * obj->momentum * (t_near - (1.f / (movScalar * 0.5f)));
                // pivot = Q * (1.f - t_near) * normal;

                if ( !__isnanf(t_near) && !__isinff(t_near)) {

                    Mat4x4 trans = translationMatrix(pivot[0], pivot[1], pivot[2]);
                    obj->v = setvecsarrayxm(obj->v, obj->v_indexes, trans);
                    obj->n = setvecsarrayxm(obj->n, obj->n_indexes, trans);

                    obj->pivot += pivot;
                }

                obj->momentum *= s->m[inner_inx].mass;
                float dot =  dot_product(normal, obj->mvdir);
                obj->mvdir = obj->mvdir - (dot * normal);
                continue;
            }
            obj->overlap = 0;
        }
    }
}

const void applyForces(Scene *s) {
    Mat4x4 trans;
    for (int i = 0; i < s->m_indexes; i++) {

        if ( s->m[i].type !=  Terrain ) {

            addMeshToQuad(&s->m[i]);

            // if ( s->m[i].momentum < 0 ) {
            //     s->m[i].momentum = s->m[i].roll = 0.f;
            //     continue;
            // }

            // if ( s->m[i].momentum >= 0 ) {
                // printf("Momentum: %f\n", s->m[i].momentum);
                // s->m[i].momentum *= s->m[i].mass;
                // s->m[i].momentum -= DeltaTime;
                // if ( s->m[i].momentum <= 0.1 ) {
                //     s->m[i].momentum = s->m[i].roll = 0.f;
                //     continue;
                // }
                s->m[i].falling_time += DeltaTime;
                const vec4f pull_point = { 0.f, -1.f, 0.f };
                const float g_accelaration = (9.81f * s->m[i].falling_time) * s->m[i].mass;

                vec4f velocity = (s->m[i].mvdir * s->m[i].momentum);
                // vec4f velocity = (pull_point * g_accelaration) + (s->m[i].mvdir * s->m[i].momentum);
                // logVec4f(velocity);
                // s->m[i].mvdir = norm_vec(velocity);

                // velocity = s->m[i].mvdir * s->m[i].momentum;
                // logVec4f(velocity);

                // EnvironmentCollision(&tf, s, &s->m[i]);
                // objectEnvironmentCollision(&tf, s, &s->m[Player_1], velocity);
                if ( s->m[i].id == Player_1 && !objectEnvironmentCollision(&tf, s, &s->m[Player_1], velocity) ) {
                    // printf("Collision 1\n");
                    // velocity = (s->m[i].mvdir * s->m[i].momentum);
                    vec4f axis = { 1.f, 0.f, 0.f };

                    if (s->m[i].roll) {
                        s->m[i].roll = s->m[i].momentum * 10;

                        Quat xrot = rotationQuat(s->m[i].roll, axis);
                        Mat4x4 m = MatfromQuat(xrot, s->m[i].pivot);
                        trans = mxm(m, translationMatrix(velocity[0], velocity[1], velocity[2]));

                        s->m[i].Q = multiplyQuats(s->m[i].Q, xrot);
                    } else {
                        trans = translationMatrix(velocity[0], velocity[1], velocity[2]);
                    }

                    s->m[i].v = setvecsarrayxm(s->m[i].v, s->m[i].v_indexes, trans);
                    s->m[i].n = setvecsarrayxm(s->m[i].n, s->m[i].n_indexes, trans);

                    s->m[i].pivot += velocity;
                }
                // EnvironmentCollision(&tf, s, &s->m[i]);
                // objectEnvironmentCollision(&tf, s, &s->m[Player_1], DeltaTime);
                // if ( !s->m[i].grounded )
                    objectTerrainCollision(&s->m[Terrain_1], &s->m[i]);
                // EnvironmentCollision(&tf, s, &s->m[i]);
                // objectEnvironmentCollision(&tf, s, &s->m[Player_1], pivot);
            // }
        }
        // s->m[i].momentum *= s->m[i].mass;
        // s->m[i].momentum = 0;
        s->m[i].momentum -= DeltaTime;
        if ( s->m[i].momentum < 0 ) {
            s->m[i].momentum = s->m[i].roll = 0.f;
            continue;
        }
    }
}
const void applyGravity(Scene *s) {
    Mat4x4 trans;
    /* Apply Gravitanional forces to Specific or all meshes. EXCEPT TERRAIN. */
    for (int i = 0; i < s->m_indexes; i++) {

        if ( s->m[i].type != Terrain ) {

            addMeshToQuad(&s->m[i]);

            // if ( (!s->m[i].grounded || s->m[i].momentum) && !s->m[i].overlap ) {

            //     s->m[i].falling_time += DeltaTime;
            //     const vec4f pull_point = { 0.f, -1.f, 0.f };
            //     const float g_accelaration = 9.81f * s->m[i].falling_time;

            //     vec4f pivot = (pull_point * g_accelaration) + (s->m[i].mvdir * s->m[i].momentum);
            //     s->m[i].mvdir = norm_vec(pivot);

            //     trans = translationMatrix(pivot[0], pivot[1], pivot[2]);
            //     s->m[i].v = setvecsarrayxm(s->m[i].v, s->m[i].v_indexes, trans);
            //     s->m[i].n = setvecsarrayxm(s->m[i].n, s->m[i].n_indexes, trans);

            //     s->m[i].pivot += pivot;

            //     objectTerrainCollision(&s->m[Terrain_1], &s->m[i]);
            // }
        }
    }
}



