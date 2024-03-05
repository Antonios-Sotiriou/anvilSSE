#include "headers/collision_detection.h"

#include "headers/logging.h"
// extern Window mainwin;
// extern Display *displ;
// extern GC gc;
// extern Mat4x4 worldMat;
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
const void objectEnvironmentCollision(TerrainInfo *tf, Scene *s, Mesh *obj) {
    if (obj->quadIndex < 0) {
        fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", obj->quadIndex);
        return;
    }

    const int num_of_members = tf->quads[obj->quadIndex].mems_indexes;
    obj->BB = getDimensionsLimits(obj->v, obj->v_indexes);

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


                    // printf("\nCollision Detected ids %d, %d!", obj->id, s->m[inner_inx].id);
                    s->m[inner_inx].momentum = obj->momentum;
                    s->m[inner_inx].mvdir = obj->mvdir;
                    obj->momentum *= s->m[inner_inx].mass;

                    // if (!s->m[inner_inx].momentum)
                    //     s->m[inner_inx].momentum += DeltaTime;
                    // s->m[inner_inx].pivot[2] += 10;

                    // vec4f pivot = obj->mvdir * obj->momentum;
                    // vec4f pt = { 0 };
                    // Mat4x4 m = MatfromQuat(obj->Q, pt);
                    // Mat4x4 trans = mxm(m, translationMatrix(pivot[0], pivot[1], pivot[2]));

                    // obj->v = setvecsarrayxm(obj->v, obj->v_indexes, trans);
                    // obj->n = setvecsarrayxm(obj->n, obj->n_indexes, trans);

                    // obj->pivot -= pivot;
                    // logDm(obj->BB);
                    // logDm(s->m[inner_inx].BB);
                    // obj->collide = 1;
                    // s->m[inner_inx].collide = 1;

                    // printf("obj minZ %f  maxZ %f    m minZ %f  maxT %f\n", obj->BB.minZ, obj->BB.maxZ, m.BB.minZ, m.BB.maxZ);
                    // objectEnvironmentCollision(tf, s, &s->m[tf->quads[obj->quadIndex].mems[i]]);
                    // printf("momentrum %f    %f    DeltaTime: %f\n", obj->momentum, s->m[inner_inx].momentum, DeltaTime);
                }
            }
        }
    }
    // for (int i = 0; i < num_of_members; i++) {
    //     s->m[tf->quads[obj->quadIndex].mems[i]].collide = 0;
    // }
    // printf("\n");
}



