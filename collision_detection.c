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

    for (int i = 0; i < num_of_members; i++) {
        
        int outer_inx = tf->quads[obj->quadIndex].mems[i];
        s->m[outer_inx].BB = getDimensionsLimits(s->m[outer_inx].v, s->m[outer_inx].v_indexes);
        // printf("\nChecking collisions obj->id: %d --> ", s->m[outer_inx].id);

        for (int j = 0; j < num_of_members; j++) {

            int inner_inx = tf->quads[obj->quadIndex].mems[j];

            if ( s->m[inner_inx].id != s->m[outer_inx].id ) {
                s->m[inner_inx].BB = getDimensionsLimits(s->m[inner_inx].v, s->m[inner_inx].v_indexes);
                printf("%d ", s->m[inner_inx].id);

                if (s->m[outer_inx].BB.minZ > s->m[inner_inx].BB.minZ && s->m[outer_inx].BB.minZ < s->m[inner_inx].BB.maxZ || 
                    s->m[outer_inx].BB.maxZ > s->m[inner_inx].BB.minZ && s->m[outer_inx].BB.maxZ < s->m[inner_inx].BB.maxZ ||
                    s->m[outer_inx].BB.minZ < s->m[inner_inx].BB.minZ && s->m[outer_inx].BB.maxZ > s->m[inner_inx].BB.maxZ) {

                    if (s->m[outer_inx].BB.minX > s->m[inner_inx].BB.minX && s->m[outer_inx].BB.minX < s->m[inner_inx].BB.maxX || 
                        s->m[outer_inx].BB.maxX > s->m[inner_inx].BB.minX && s->m[outer_inx].BB.maxX < s->m[inner_inx].BB.maxX || 
                        s->m[outer_inx].BB.minX < s->m[inner_inx].BB.minX && s->m[outer_inx].BB.maxX > s->m[inner_inx].BB.maxX) {

                        // printf("\nCollision Detected ids %d, %d!", s->m[outer_inx].id, s->m[inner_inx].id);
                        s->m[inner_inx].momentum = s->m[outer_inx].momentum;
                        s->m[inner_inx].mvdir = s->m[outer_inx].mvdir;
                        s->m[outer_inx].momentum = 0;//*= s->m[inner_inx].mass;

                        if (!s->m[inner_inx].momentum)
                            s->m[inner_inx].momentum += DeltaTime;
                        // s->m[inner_inx].pivot[2] += 10;

                        // vec4f pivot = s->m[outer_inx].mvdir * s->m[outer_inx].momentum;
                        // vec4f pt = { 0 };
                        // Mat4x4 m = MatfromQuat(s->m[outer_inx].Q, pt);
                        // Mat4x4 trans = mxm(m, translationMatrix(pivot[0], pivot[1], pivot[2]));

                        // s->m[outer_inx].v = setvecsarrayxm(s->m[outer_inx].v, s->m[outer_inx].v_indexes, trans);
                        // s->m[outer_inx].n = setvecsarrayxm(s->m[outer_inx].n, s->m[outer_inx].n_indexes, trans);

                        // s->m[outer_inx].pivot -= pivot;
                        // logDm(obj->BB);
                        // logDm(s->m[inner_inx].BB);
                        // s->m[outer_inx].collide = 1;
                        // s->m[inner_inx].collide = 1;

                        // printf("obj minZ %f  maxZ %f    m minZ %f  maxT %f\n", obj->BB.minZ, obj->BB.maxZ, m.BB.minZ, m.BB.maxZ);
                        // objectEnvironmentCollision(tf, s, &s->m[tf->quads[obj->quadIndex].mems[i]]);
                        printf("momentrum %f    %f    DeltaTime: %f\n", obj->momentum, s->m[inner_inx].momentum, DeltaTime);
                    }
                }
            }
        }
        printf("\n");
        // s->m[outer_inx].collide = 0;
    }
    // for (int i = 0; i < num_of_members; i++) {
    //     s->m[tf->quads[obj->quadIndex].mems[i]].collide = 0;
    // }
    // printf("\n");
}



