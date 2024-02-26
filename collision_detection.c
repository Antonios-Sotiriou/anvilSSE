#include "headers/collision_detection.h"

#include "headers/logging.h"
extern Window mainwin;
extern Display *displ;
extern GC gc;
extern Mat4x4 worldMat;
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
    const int num_of_members = tf->quads[obj->quadIndex].mems_indexes;
    obj->BB = getDimensionsLimits(obj->v, obj->v_indexes);
    Mesh m;
    for (int i = 0; i < num_of_members; i++) {
        m = s->m[tf->quads[obj->quadIndex].mems[i]];
        m.BB = getDimensionsLimits(m.v, m.v_indexes);

        if (obj->BB.minZ > m.BB.minZ && obj->BB.minZ < m.BB.maxZ || 
            obj->BB.maxZ > m.BB.minZ && obj->BB.maxZ < m.BB.maxZ ||
            obj->BB.minZ < m.BB.minZ && obj->BB.maxZ > m.BB.maxZ)

            if (obj->BB.minX > m.BB.minX && obj->BB.minX < m.BB.maxX || 
                obj->BB.maxX > m.BB.minX && obj->BB.maxX < m.BB.maxX || 
                obj->BB.minX < m.BB.minX && obj->BB.maxX > m.BB.maxX) {

                printf("We have possible collision\n");
                s->m[tf->quads[obj->quadIndex].mems[i]].rahm = obj->rahm;
                s->m[tf->quads[obj->quadIndex].mems[i]].mvdir = obj->mvdir;
                obj->rahm = 0;

                // objectEnvironmentCollision(tf, s, &s->m[tf->quads[obj->quadIndex].mems[i]]);
            }
    }
}



