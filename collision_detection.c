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
    printf("collision check\n");
    const int num_of_members = tf->quads[obj->quadIndex].mems_indexes;
    obj->BB = getDimensionsLimits(obj->v, obj->v_indexes);
    int collide = 0;

    for (int i = 0; i < num_of_members; i++) {

        int inner_inx = tf->quads[obj->quadIndex].mems[i];

        if ( s->m[inner_inx].id != obj->id ) {
            s->m[inner_inx].BB = getDimensionsLimits(s->m[inner_inx].v, s->m[inner_inx].v_indexes);

            if (obj->BB.minZ > s->m[inner_inx].BB.minZ && obj->BB.minZ < s->m[inner_inx].BB.maxZ ||
                obj->BB.maxZ > s->m[inner_inx].BB.minZ && obj->BB.maxZ < s->m[inner_inx].BB.maxZ ||
                obj->BB.minZ < s->m[inner_inx].BB.minZ && obj->BB.maxZ > s->m[inner_inx].BB.maxZ) {

                if (obj->BB.minX > s->m[inner_inx].BB.minX && obj->BB.minX < s->m[inner_inx].BB.maxX || 
                    obj->BB.maxX > s->m[inner_inx].BB.minX && obj->BB.maxX < s->m[inner_inx].BB.maxX || 
                    obj->BB.minX < s->m[inner_inx].BB.minX && obj->BB.maxX > s->m[inner_inx].BB.maxX) {



                    s->m[i].momentum += (DeltaTime * 0.5);
                    vec4f pivot = -s->m[i].mvdir * s->m[i].momentum;
                    Mat4x4 trans = translationMatrix(pivot[0], pivot[1], pivot[2]);

                    s->m[i].v = setvecsarrayxm(s->m[i].v, s->m[i].v_indexes, trans);
                    s->m[i].n = setvecsarrayxm(s->m[i].n, s->m[i].n_indexes, trans);

                    s->m[i].pivot += pivot;

                    // i = i == 0 ? 0 : i--;
                    s->m[i].momentum = 0;

                    printf("objectCollision  ");
                    logVec4f(pivot);

                    // printf("X: %f\n", obj->BB.maxX - s->m[inner_inx].BB.minX);
                    // s->m[inner_inx].momentum = obj->momentum;
                    // s->m[inner_inx].mvdir = obj->mvdir;
                    // obj->momentum *= s->m[inner_inx].mass;
                }
            }
        }
    }
}



