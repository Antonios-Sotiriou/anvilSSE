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

    // vec4f D = (obj->pivot + velocity) - obj->pivot;

    obj->BB = getDimensionsLimits(obj->v, obj->v_indexes);

    float tnearx, tneary, tnearz, tfarx, tfary, tfarz;

    system("clear\n");
    // printQuad(obj->quadIndex);
    // logDm(obj->BB);
    // printf("Pivot before: ");
    // logVec4f(obj->pivot);
    // logDm(s->m[3].BB);

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

            // if (__isnanf(tnearx * tneary * tnearz * tfarx * tfary * tfarz))
            //     continue;

            // tnearx = __isnanf(tnearx) ? printf("tnearx NAN\n"), 0.f : tnearx;
            // tneary = __isnanf(tneary) ? printf("tneary NAN\n"), 0.f : tneary;
            // tnearz = __isnanf(tnearz) ? printf("tnearz NAN\n"), 0.f : tnearz;
            // tfarx = __isnanf(tfarx) ? printf("tfarx NAN\n"), 0.f : tfarx;
            // tfary = __isnanf(tfary) ? printf("tfary NAN\n"), 0.f : tfary;
            // tfarz = __isnanf(tfarz) ? printf("tfarz NAN\n"), 0.f : tfarz;


            if (tnearx > tfarx) swap(&tnearx, &tfarx, 4);
            if (tnearz > tfarz) swap(&tnearz, &tfarz, 4);

            // if ( (tnearx == tnearz) ) {
            //     getc(stdin);
            //     continue;
            // }

            if (tnearx > tfarz || tnearz > tfarx) {
                // printf("tnearx > tfarz || tnearz > tfarx!\n");
                continue;
            }

            float t_near = tnearx > tnearz ? tnearx : tnearz;
            float t_far = tfarx < tfarz ? tfarx : tfarz;

            if (t_far < 0) { 
                // printf("t_far <= 0!\n");
                continue;
            }

            vec4f normal = { 0.f };
            if ( tnearx > tnearz ) {
                if ( velocity[0] < 0 ) {
                    normal[0] = 1.f;
                    printf("Right\n");
                } else {     
                    normal[0] = -1.f;
                    printf("Left\n");
                }
            } else if ( tnearx <= tnearz ) {
                if ( velocity[2] < 0 ) {
                    normal[2] = 1.f;
                    printf("Front\n");
                } else {
                    normal[2] = -1.f;
                    printf("Back\n");
                }
            }

            vec4f pivot = { 0 };
            printf("minx: %d    minz: %d\n", minx, minz);
            printf("maxx: %d    maxz: %d\n", maxx, maxz);

            printf("tnnearx: %f    tnnearz: %f\n", tnearx, tnearz);
            printf("tfarx  : %f    tfarz  : %f\n", tfarx, tfarz);

           if ( __isinf(tnearz))
               printf("nearz: %f\n", tnearz * 0.f);

            if ( t_near == 0.f ) {
                printf("Sliding...\n");
                float dot =  dot_product(normal, obj->mvdir);
                obj->mvdir = obj->mvdir - (dot * normal);
                // getc(stdin);
                return 0;
            } else if ( ((t_near > 0.f) && (t_near <= 1.f)) && (!__isnanf(tnearx * tneary * tnearz * tfarx * tfary * tfarz)) ) {
                printf("Collision 1 t_near: %f\n", t_near);

                pivot = velocity * (t_near);

                Mat4x4 trans = translationMatrix(pivot[0], pivot[1], pivot[2]);
                obj->v = setvecsarrayxm(obj->v, obj->v_indexes, trans);
                obj->n = setvecsarrayxm(obj->n, obj->n_indexes, trans);

                obj->pivot += pivot;

                // printf("Pivot change: ");
                // logVec4f(pivot);
                // printf("Pivot after : ");
                // logVec4f(obj->pivot);
                // getc(stdin);

                obj->momentum *= s->m[inner_inx].mass;
                float dot =  dot_product(normal, obj->mvdir);
                obj->mvdir = obj->mvdir - (dot * normal);

                return 1;
            }
        }
    }
    printf("Nothing happened\n");
    // getc(stdin);
    return 0;
}



