#include "headers/collision_detection.h"

#include "headers/logging.h"
extern float DeltaTime;

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
const void objectEnvironmentCollision(TerrainInfo *tf, Scene *s, Mesh *obj, const float dt) {
    if (obj->quadIndex < 0) {
        // fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", obj->quadIndex);
        return;
    }

    // obj->momentum -= DeltaTime;
    vec4f Q = obj->mvdir * obj->momentum;
    vec4f D = (obj->pivot + Q) - obj->pivot;

    obj->BB = getDimensionsLimits(obj->v, obj->v_indexes);

    float tnearx, tnearz, tfarx, tfarz;

    const int num_of_members = tf->quads[obj->quadIndex].mems_indexes;
    for (int i = 0; i < num_of_members; i++) {

        int inner_inx = tf->quads[obj->quadIndex].mems[i];
        // printf("obj id: %d\n", tf->quads[obj->quadIndex].mems[i]);
        if ( s->m[inner_inx].id != obj->id ) {
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
            // printf("minx: %f    miny: %f    minz: %f    maxx: %f    maxy: %f    maxz: %f\n", minx, miny, minz, maxx, maxy, maxz);

            if (tnearx > tfarx) swap(&tnearx, &tfarx, 4);
            if (tnearz > tfarz) swap(&tnearz, &tfarz, 4);


            if (tnearx > tfarz || tnearz > tfarx) {
                // printf("Unable to Collide!\n");
                continue;
            }

            float t_near = tnearx > tnearz ? tnearx : tnearz;
            float t_far = tfarx < tfarz ? tfarx : tfarz;

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
            }

            if (t_near <= 1.0f) {
                obj->momentum *= s->m[inner_inx].mass;
                // obj->momentum = 0;

                s->m[inner_inx].mvdir = obj->mvdir;
                s->m[inner_inx].momentum = obj->momentum;

                // float dot = dot_product(obj->mvdir, normal);
                obj->mvdir = normal + obj->mvdir;//norm_vec(dot * normal);
                // printf("Collision: ");
                // logVec4f(obj->mvdir);

                Q[0] = fabsf(Q[0]);
                Q[1] = fabsf(Q[1]);
                Q[2] = fabsf(Q[2]);

                vec4f pivot = Q * (1.f - t_near) * normal;

                Mat4x4 trans = translationMatrix(pivot[0], pivot[1], pivot[2]);
                obj->v = setvecsarrayxm(obj->v, obj->v_indexes, trans);
                obj->n = setvecsarrayxm(obj->n, obj->n_indexes, trans);

                obj->pivot += pivot;
                return;
            }
            // printf("t_near: %f\n", t_near);
            // printf("momentum: %f\n", obj->momentum);
            // logVec4f(normal);
        }
    }
}



