#include "../../headers/components/physics.h"

/* Defined in main.c. */
extern vec4f gravity_epicenter;

const void applyPhysics(Scene *s) {
    Mat4x4 trans_mat;
    for (int i = 0; i < s->m_indexes; i++) {

        if ( (s->m[i].type != Terrain) && (s->m[i].type != Celestial) ) {

            initMeshQuadInfo(&s->m[Terrain_1], &s->m[i]);

            /* Apply Gravitational force. */
            // if (!s->m[i].grounded) {
            //     float g_accelaration = (9.81f * (s->m[i].falling_time * s->m[i].falling_time));
            //     s->m[i].velocity = (gravity_epicenter * g_accelaration) + (s->m[i].velocity);
            //     s->m[i].falling_time += DeltaTime;
            // }

            // if ( s->m[i].type == Player )
            //     terrainHeightDifference(&s->m[Terrain_1], &s->m[i]);

            // int collide = 1;
            // while (collide) {
            //     sortCollisions(&scene.t, s, &s->m[i]);

            //     collide = aabbCollision(&scene.t, s, &s->m[i]);
            // }

            if ( s->m[i].type == Player ) {
                // obbCollision(&s->m[6]);
                obbCollision(&s->m[1]);
            }

            if ( s->m[i].rot_angle ) {
                rotationCollision(&scene.t, s, &s->m[i]);
            }

            // if (!simpleTerrainCollision(&s->m[Terrain_1], &s->m[i])) {
            // if (s->m[i].type != Player)
            if (!terrainCollision(&s->m[Terrain_1], &s->m[i])) {
                trans_mat = translationMatrix(s->m[i].velocity[0], s->m[i].velocity[1], s->m[i].velocity[2]);

                setvecsarrayxm(s->m[i].cd.v, 4, trans_mat);
                setvecsarrayxm(s->m[i].bbox.v, s->m[i].bbox.v_indexes, trans_mat);
                setfacesarrayxm(s->m[i].bbox.f, s->m[i].bbox.f_indexes, trans_mat);
            }
        }
    }
}



