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
        fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", obj->quadIndex);
        return;
    }
    // printf("collision check\n");
    /* initiallization stage to get cache and future object positions. */
    vec4f *vecs_arr = malloc(obj->v_indexes * 16);
    memcpy(vecs_arr, obj->v, obj->v_indexes * 16);

    obj->momentum -= DeltaTime;
    vec4f pivot = obj->mvdir * obj->momentum;
    Mat4x4 trans = translationMatrix(pivot[0], pivot[1], pivot[2]);
    vecs_arr = vecsarrayxm(obj->v, obj->v_indexes, trans);

    obj->BB = getDimensionsLimits(obj->v, obj->v_indexes);


    // printf("Starting Point ");
    // logVec4f(obj->pivot);
    // printf("Ending Point ");
    // logVec4f(obj->pivot + pivot);

    float tminx, tminy, tmaxx, tmaxy;

    int first_collision = 0;
    const int num_of_members = tf->quads[obj->quadIndex].mems_indexes;
    for (int i = 0; i < num_of_members; i++) {

        int inner_inx = tf->quads[obj->quadIndex].mems[i];

        if ( s->m[inner_inx].id != obj->id ) {
            s->m[inner_inx].BB = getDimensionsLimits(s->m[inner_inx].v, s->m[inner_inx].v_indexes);

            // vec4f dir = obj->pivot - pivot;

            tminx = (s->m[inner_inx].BB.minX - obj->BB.maxX) / (obj->BB.maxX - obj->BB.maxX);
            tminy = (s->m[inner_inx].BB.minZ - obj->BB.maxZ) / (obj->BB.maxZ - obj->BB.maxZ);
            tmaxx = (s->m[inner_inx].BB.maxX - obj->BB.maxX) / (obj->BB.maxX - obj->BB.maxX);
            tmaxy = (s->m[inner_inx].BB.maxZ - obj->BB.maxZ) / (obj->BB.maxZ - obj->BB.maxZ);
            printf("tminx: %f    tminy: %f    tmaxx: %f    tmaxy: %f\n", tminx, tminy, tmaxx, tmaxy);

            // logVec4f(obj->pivot + pivot);
            // logVec4f((obj->pivot + pivot) * t);

            if (obj->BB.minZ > s->m[inner_inx].BB.minZ && obj->BB.minZ < s->m[inner_inx].BB.maxZ ||
                obj->BB.maxZ > s->m[inner_inx].BB.minZ && obj->BB.maxZ < s->m[inner_inx].BB.maxZ ||
                obj->BB.minZ < s->m[inner_inx].BB.minZ && obj->BB.maxZ > s->m[inner_inx].BB.maxZ ) {

                if (obj->BB.minX > s->m[inner_inx].BB.minX && obj->BB.minX < s->m[inner_inx].BB.maxX ||
                    obj->BB.maxX > s->m[inner_inx].BB.minX && obj->BB.maxX < s->m[inner_inx].BB.maxX ||
                    obj->BB.minX < s->m[inner_inx].BB.minX && obj->BB.maxX > s->m[inner_inx].BB.maxX ) {



                    // obj->momentum += (DeltaTime * 0.5);
                    // vec4f pivot = (obj->mvdir * obj->momentum) * t;
                    // Mat4x4 trans = translationMatrix(pivot[0], pivot[1], pivot[2]);

                    // obj->v = setvecsarrayxm(obj->v, obj->v_indexes, trans);
                    // obj->n = setvecsarrayxm(obj->n, obj->n_indexes, trans);

                    // obj->pivot += pivot;
                    // logVec4f((obj->pivot + obj->scale) - (s->m[inner_inx].pivot + s->m[inner_inx].scale));
                    // obj->momentum = 0;

                    printf("objectCollision  ");
                    // exit(0);
                    // logVec4f(pivot);

                    // printf("X: %f\n", obj->BB_next.maxX - s->m[inner_inx].BB_next.minX);
                    // s->m[inner_inx].momentum = obj->momentum;
                    // s->m[inner_inx].mvdir = obj->mvdir;
                    // obj->momentum *= s->m[inner_inx].mass;
                }
            }
        }
    }
    free(vecs_arr);
}



