#include "headers/collision_detection.h"

#include "headers/logging.h"
extern float DeltaTime, movScalar;

const void objectTerrainCollision(Mesh *terrain, Mesh *object) {
    const float height = getTerrainHeight(terrain, object->pivot, object);
    float height_diff = height - (object->pivot[1] - object->scale);
    if (height_diff >= 0) {
        object->grounded = 1;
        object->falling_time = 0;
    }
    if (object->grounded) {
        Mat4x4 dr = translationMatrix(0, height_diff, 0);
        object->v = setvecsarrayxm(object->v, object->v_indexes, dr);
        object->n = setvecsarrayxm(object->n, object->n_indexes, dr);
        object->pivot[1] += height_diff;
    }
}
const int objectEnvironmentCollision(TerrainInfo *tf, Scene *s, Mesh *obj, vec4f velocity) {
    if (obj->quadIndex < 0) {
        // fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", obj->quadIndex);
        return 0;
    }

    obj->BB = getDimensionsLimits(obj->v, obj->v_indexes);

    float tnearx, tneary, tnearz, tfarx, tfary, tfarz;

    system("clear\n");

    const int num_of_members = tf->quads[obj->quadIndex].mems_indexes;
    for (int i = 0; i < num_of_members; i++) {

        int inner_inx = tf->quads[obj->quadIndex].mems[i];

        if ( s->m[inner_inx].id != obj->id ) {
            s->m[inner_inx].BB = getDimensionsLimits(s->m[inner_inx].v, s->m[inner_inx].v_indexes);

            int minx = (s->m[inner_inx].BB.minX - (obj->pivot[0] - obj->BB.minX)) + 0.5;
            int miny = (s->m[inner_inx].BB.minY - (obj->pivot[1] - obj->BB.minY)) + 0.5;
            int minz = (s->m[inner_inx].BB.minZ - (obj->pivot[2] - obj->BB.minZ)) + 0.5;
            int maxx = (s->m[inner_inx].BB.maxX - (obj->pivot[0] - obj->BB.maxX)) + 0.5;
            int maxy = (s->m[inner_inx].BB.maxY - (obj->pivot[1] - obj->BB.maxY)) + 0.5;
            int maxz = (s->m[inner_inx].BB.maxZ - (obj->pivot[2] - obj->BB.maxZ)) + 0.5;

            tnearx = (minx - obj->pivot[0]) / velocity[0];
            tneary = (miny - obj->pivot[1]) / velocity[1];
            tnearz = (minz - obj->pivot[2]) / velocity[2];
            tfarx = (maxx - obj->pivot[0]) / velocity[0];
            tfary = (maxy - obj->pivot[1]) / velocity[1];
            tfarz = (maxz - obj->pivot[2]) / velocity[2];

            // const float check_nan = tnearx * tneary * tnearz * tfarx * tfary * tfarz;
            // printf("\ncheck_nan: %f\n", check_nan);

            if (tnearx > tfarx) swap(&tnearx, &tfarx, 4);
            if (tneary > tfary) swap(&tneary, &tfary, 4);
            if (tnearz > tfarz) swap(&tnearz, &tfarz, 4);

            if (tnearx > tfarz || tnearz > tfarx) {
                // printf("tnearx > tfarz || tnearz > tfarx!\n");
                continue;
            }

            float t_near = tnearx > tnearz ? tnearx : tnearz;
            float t_far = tfarx < tfarz ? tfarx : tfarz;

            /* ##################### Y ############################ */
            if (t_near > tfary || tneary > t_far) {
                // printf("t_near > tfary || tneary > t_far!\n");
                // getc(stdin);
                continue;
            }

            if (tneary > t_near) {
                t_near = tneary;
                // printf("tneary > t_near!\n");
                // getc(stdin);
            }
            if (tfary < t_far) {
                t_far = tfary;
                // printf("tfary < t_far!\n");
                // getc(stdin);
            }
            /* ##################### Y ############################ */

            if (t_far < 0 || t_near < 0) { 
                // printf("t_far <= 0!\n");
                continue;
            }

            vec4f normal = { 0.f };
            // if (!__isnanf(tnearx * tneary * tnearz * tfarx * tfary * tfarz)) {
                if ( tnearx >= tneary && tnearx >= tnearz ) {
                    if ( velocity[0] < 0 ) {
                        normal[0] = 1.f;
                        printf("Right\n");
                    } else if ( velocity[0] > 0 ) {     
                        normal[0] = -1.f;
                        printf("Left\n");
                    }
                } else if ( tneary >= tnearx && tneary >= tnearz ) {
                    if ( velocity[1] < 0 ) {
                        normal[1] = 1.f;
                        printf("Up\n");
                    } else if ( velocity[1] > 0 ) {
                        normal[1] = -1.f;
                        printf("Down\n");
                    }
                } else if ( tnearz >= tnearx && tnearz >= tneary ) {
                    if ( velocity[2] < 0 ) {
                        normal[2] = 1.f;
                        printf("Back\n");
                    } else if ( velocity[2] > 0 ) {
                        normal[2] = -1.f;
                        printf("Front\n");
                    }
                } else {
                    printf("EXTRA CASE\n");
                }
            // }

            // printf("minx: %d    miny: %d    minz: %d\n", minx, miny, minz);
            // printf("maxx: %d    maxy: %d    maxz: %d\n", maxx, maxy, maxz);

            printf("tnearx: %f    tneary: %f    tnearz: %f\n", tnearx, tneary, tnearz);
            printf("tfarx  : %f    tfary  : %f    tfarz  : %f\n", tfarx, tfary, tfarz);
            // logVec4f(dot_product(obj->pivot, s->m[inner_inx].pivot) * velocity);

            if ( t_near == 0.f ) {
                printf("Sliding:   t_near = %f", t_near);
                // if ( tneary == 0.f || tfary == 0.f )
                //     return 1;
                float dot =  dot_product(normal, obj->mvdir);
                obj->mvdir = obj->mvdir - (dot * normal);
                // logVec4f(obj->mvdir);
                // getc(stdin);
                if ( tneary == 0.f || tfary == 0.f )
                    return 1;
                return 0;
            } else if ( ((t_near > 0.f) && (t_near <= 1.f)) && (!__isnanf(tnearx * tneary * tnearz * tfarx * tfary * tfarz) || (tnearx == 0 || tneary == 0 || tnearz == 0)) ) {
                printf("Collision: t_near = %f", t_near);
                // && (!__isnanf(tnearx * tneary * tnearz * tfarx * tfary * tfarz))
                velocity *= t_near;

                Mat4x4 trans = translationMatrix(velocity[0], velocity[1], velocity[2]);
                obj->v = setvecsarrayxm(obj->v, obj->v_indexes, trans);
                obj->n = setvecsarrayxm(obj->n, obj->n_indexes, trans);

                obj->pivot += velocity;

                obj->momentum *= s->m[inner_inx].mass;
                float dot =  dot_product(normal, obj->mvdir);
                obj->mvdir = obj->mvdir - (dot * normal);
                // logVec4f(obj->mvdir);
                // getc(stdin);
                return 2;
            }
        }
    }
    printf("Nothing happened\n");
    return 0;
}



