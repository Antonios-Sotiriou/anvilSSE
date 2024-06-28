#include "../headers/physics.h"

/* Defined in main.c. */
extern TerrainInfo tf;
extern vec4f gravity_epicenter;

#include "../headers/logging.h"

const void applyForces(Scene *s) {
    Mat4x4 trans;
    // system("clear\n");
    for (int i = 0; i < s->m_indexes; i++) {

        if ( s->m[i].type !=  Terrain ) {

            initMeshQuadInfo(&s->m[Terrain_1], &s->m[i]);

            s->m[i].momentum -= DeltaTime;
            if ( s->m[i].momentum < 0 ) {
                s->m[i].momentum = s->m[i].roll = 0.f;
                // continue;
            }

            rotationCollision(&tf, s, &s->m[Player_1]);

            if ( s->m[i].momentum > 0 ) {
                s->m[i].falling_time += DeltaTime;
                
                const float g_accelaration = (9.81f * s->m[i].falling_time) * s->m[i].mass;

                vec4f velocity[1];
                velocity[0] = (gravity_epicenter * g_accelaration) + (s->m[i].mvdir * s->m[i].momentum);

                if ( s->m[i].id == Player_1 ) {

                    int collide = 1;
                    while (collide) {
                        sortObjectCollisions(&tf, s, &s->m[Player_1], velocity);

                        collide = objectEnvironmentCollision(&tf, s, &s->m[Player_1], velocity);
                    }
                    
                    // vec4f axis = { 1.f, 0.f, 0.f };
                    // if (s->m[i].roll) {
                    //     s->m[i].roll = s->m[i].momentum * 10;

                    //     Quat xrot = rotationQuat(s->m[i].roll, axis);

                    //     s->m[i].Q = multiplyQuats(s->m[i].Q, xrot);
                    // }

                    trans = translationMatrix(velocity[0][0], velocity[0][1], velocity[0][2]);

                    s->m[i].bbox.v = setvecsarrayxm(s->m[i].bbox.v, s->m[i].bbox.v_indexes, trans);

                    s->m[i].pivot += velocity[0];
                }
            }
            if (s->m[i].type != Celestial)
                objectTerrainCollision(&s->m[Terrain_1], &s->m[i]);
        }
    }
}



