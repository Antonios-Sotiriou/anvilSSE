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

    vec4f D = (obj->pivot + velocity) - obj->pivot;

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

            // printf("minx: %f    miny: %f    minz: %f    maxx: %f    maxy: %f    maxz: %f\n", minx, miny, minz, maxx, maxy, maxz);

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
                if ( D[0] < 0 )
                    normal[0] = 1.f;
                else     
                    normal[0] = -1.f;
            } else if ( tnearx < tnearz ) {
                if ( D[2] < 0 )
                    normal[2] = 1.f;
                else
                    normal[2] = -1.f;
            } else {
                if ( D[1] < 0)
                    normal[1] = 1.f;
                else
                    normal[1] = -1.f;
            }

             vec4f pivot = { 0 };
            if ( t_near <= 1.f ) {
                printf("Collision 1\n");
                // printf("x: %f    y: %f    z: %f  ", tnearx, tneary, tnearz);
                // printf("x: %f    y: %f    z: %f\n", tfarx, tfary, tfarz);

                // obj->momentum = 0;
                // s->m[inner_inx].mvdir = obj->mvdir;
                // s->m[inner_inx].momentum = obj->momentum;
                if (normal[1] == 1.f) {
                    obj->overlap = 1;
                }

                velocity[0] = fabsf(velocity[0]);
                velocity[1] = fabsf(velocity[1]);
                velocity[2] = fabsf(velocity[2]);

                pivot = D * (t_near - (1.f / (movScalar * 0.5f)));
                // pivot = velocity * (1.f - t_near) * normal;

                if ( !__isnanf(t_near) && !__isinff(t_near)) {

                    Mat4x4 trans = translationMatrix(pivot[0], pivot[1], pivot[2]);
                    obj->v = setvecsarrayxm(obj->v, obj->v_indexes, trans);
                    obj->n = setvecsarrayxm(obj->n, obj->n_indexes, trans);

                    obj->pivot += pivot;
                }

                obj->momentum *= s->m[inner_inx].mass;
                float dot =  dot_product(normal, obj->mvdir);
                obj->mvdir = obj->mvdir - (dot * normal);
                // logVec4f(normal);
                // logVec4f(obj->mvdir);
                return 1;
            }
            obj->overlap = 0;
            // logVec4f(normal);
            // logVec4f(obj->mvdir);
        }
    }
    return 0;
}



