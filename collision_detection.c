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

            // printf("\x1b[H\x1b[J");
            printf("minx: %d    miny: %d    minz: %d\n", minx, miny, minz);
            printf("maxx: %d    maxy: %d    maxz: %d\n", maxx, maxy, maxz);
            printf("Momentum: %f\n", obj->momentum);
            logVec4f(obj->mvdir);
            logVec4f(obj->pivot);

            printf("nx: %f    ny: %f    nz: %f\n", tnearx, tneary, tnearz);
            printf("fx: %f    fy: %f    fz: %f\n", tfarx, tfary, tfarz);

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

            if (t_far <= 0) { 
                // printf("Collision in negative direction!\n");
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
            } else if ( tnearx < tnearz ) {
                if ( velocity[2] < 0 ) {
                    normal[2] = 1.f;
                    printf("Front\n");
                } else {
                    normal[2] = -1.f;
                    printf("Back\n");
                }
            } else {
                if ( velocity[1] < 0) {
                    normal[0] = 0.f;
                    normal[1] = 1.f;
                    normal[2] = 0.f;
                    printf("Up\n");
                } else {
                    normal[0] = 0.f;
                    normal[1] = -1.f;
                    normal[2] = 0.f;
                    printf("Down\n");
                }
            }

            // if (t_near < 0)
            //     return 0;

            // if ( !__isnanf(t_near) && !__isinff(t_near)) {

                vec4f pivot = { 0 };
                const float bias = 1.f / (movScalar * 0.5f);
                printf("t_near: %f    bias: %f\n", t_near, t_near - bias);
                if ( t_near == (0.f) ) {
                    printf("Sliding...\n");
                    float dot =  dot_product(normal, obj->mvdir);
                    obj->mvdir = obj->mvdir - (dot * normal);
                    // getc(stdin);
                    return 0;
                // } else if ( t_near <= (1.f) ) {
                } else if ( t_near > 0.f && t_near < (1.f) ) {
                    // printf("Collision 1\n");
                    printf("Collision 1 t_near: %f\n", t_near);
                    // getc(stdin);
                    // printf("x: %f    y: %f    z: %f  ", tnearx, tneary, tnearz);
                    // printf("x: %f    y: %f    z: %f\n", tfarx, tfary, tfarz);

                    // obj->momentum = 0;
                    // s->m[inner_inx].mvdir = obj->mvdir;
                    // s->m[inner_inx].momentum = obj->momentum;
                    if (normal[1] == 1.f) {
                        obj->overlap = 1;
                    }

                    // velocity[0] = fabsf(velocity[0]);
                    // velocity[1] = fabsf(velocity[1]);
                    // velocity[2] = fabsf(velocity[2]);

                    // pivot = velocity * (t_near - (1.f / (movScalar * 0.5f)));
                    // pivot = velocity * (1.f - t_near) * normal;

                    if ( !__isnanf(t_near) && !__isinff(t_near)) {

                        pivot = velocity * (t_near);
                        // pivot = velocity * (t_near - bias);
                        // pivot = velocity * (1.f - t_near) * normal;
                    }
                    logVec4f(pivot);

                    Mat4x4 trans = translationMatrix(pivot[0], pivot[1], pivot[2]);
                    obj->v = setvecsarrayxm(obj->v, obj->v_indexes, trans);
                    obj->n = setvecsarrayxm(obj->n, obj->n_indexes, trans);

                    obj->pivot += pivot;

                    obj->momentum *= s->m[inner_inx].mass;
                    float dot =  dot_product(normal, obj->mvdir);
                    obj->mvdir = obj->mvdir - (dot * normal);
                    // logVec4f(normal);
                    // logVec4f(obj->mvdir);
                    obj->collisions++;
                    // if (obj->collisions > 1)
                    //     getc(stdin);

                    return 1;
                }
                obj->overlap = 0;
                // logVec4f(normal);
                // logVec4f(obj->mvdir);
            // }
        }
    }
    return 0;
}



