#include "../headers/physics.h"

/* Defined in main.c. */
extern TerrainInfo tf;
extern vec4f gravity_epicenter;

const void applyForces(Scene *s) {
    Mat4x4 trans;

    int rotation_collide = 0;
    for (int i = 0; i < s->m_indexes; i++) {

        if ( (s->m[i].type != Terrain) && (s->m[i].type != Celestial) ) {

            initMeshQuadInfo(&s->m[Terrain_1], &s->m[i]);

            if ( s->m[i].rot_angle ) {

                Mat4x4 tm = MatfromQuat(s->m[i].r, s->m[i].cd.v[P]);

                setvecsarrayxm(s->m[i].cd.v, 4, tm);
                setvecsarrayxm(s->m[i].bbox.v, s->m[i].bbox.v_indexes, tm);

                s->m[i].Q = multiplyQuats(s->m[i].Q, s->m[i].r);

                rotationCollision(&tf, s, &s->m[i]);
            }

            // if ( !rotation_collide )
            //     rotation_collide += rotationCollision(&tf, s, &s->m[i]);

            if ( s->m[i].momentum ) {
                s->m[i].falling_time += DeltaTime;
                
                const float g_accelaration = (9.81f * s->m[i].falling_time) * s->m[i].mass;

                vec4f velocity[1];
                velocity[0] = (gravity_epicenter * g_accelaration) + (s->m[i].mvdir * s->m[i].momentum);

                // if ( s->m[i].id == Player_1 ) {

                    int collide = 1;
                    while (collide) {
                        sortObjectCollisions(&tf, s, &s->m[i], velocity);

                        collide = objectEnvironmentCollision(&tf, s, &s->m[i], velocity);
                    }

                    trans = translationMatrix(velocity[0][0], velocity[0][1], velocity[0][2]);

                    setvecsarrayxm(s->m[i].cd.v, 4, trans);
                    setvecsarrayxm(s->m[i].bbox.v, s->m[i].bbox.v_indexes, trans);

                    // s->m[i].cd.v[P] += velocity[0];
                // }
            }
            if ( s->m[i].type != Celestial || !s->m[i].grounded )
                objectTerrainCollision(&s->m[Terrain_1], &s->m[i]);
        }
    }
}



