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

    // obj->momentum -= DeltaTime;
    vec4f Q = obj->mvdir * obj->momentum;
    vec4f D = obj->pivot + Q;

    obj->BB = getDimensionsLimits(obj->v, obj->v_indexes);

    float tnearx, tneary, tfarx, tfary;

    const int num_of_members = tf->quads[obj->quadIndex].mems_indexes;
    for (int i = 0; i < num_of_members; i++) {

        int inner_inx = tf->quads[obj->quadIndex].mems[i];

        if ( s->m[inner_inx].id != obj->id ) {
            s->m[inner_inx].BB = getDimensionsLimits(s->m[inner_inx].v, s->m[inner_inx].v_indexes);

            // vec4f dir = obj->pivot - pivot;

            tnearx = (s->m[inner_inx].BB.minX - obj->pivot[0]) / ((D[0]) - obj->pivot[0]);
            tneary = (s->m[inner_inx].BB.minZ - obj->pivot[2]) / ((D[2]) - obj->pivot[2]);
            tfarx = (s->m[inner_inx].BB.maxX - obj->pivot[0]) / ((D[0]) - obj->pivot[0]);
            tfary = (s->m[inner_inx].BB.maxZ - obj->pivot[2]) / ((D[2]) - obj->pivot[2]);
            // printf("tnearx: %f    tneary: %f    tfarx: %f    tfary: %f\n", tnearx, tneary, tfarx, tfary);

            if (tnearx > tfarx) swap(&tnearx, &tfarx, 4);
            if (tneary > tfary) swap(&tneary, &tfary, 4);


            if (tnearx > tfary || tneary > tfarx) {
                printf("Unable to Collide!\n"); 
                return;
            }

            float t_near = tnearx > tneary ? tnearx : tneary;
            float t_far = tfarx < tfary ? tfarx : tfary;

            if (t_far < 0) { 
                printf("Collision in negative direction!\n"); 
                return;
            } 

            if (t_near < 1) {
                printf("Collision Detected!\n");
                printf("t_near: %f    t_far: %f\n", t_near, t_far);
                logVec4f(obj->pivot + (Q * t_near));
            }
        }
    }
}



