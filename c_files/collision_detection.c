#include "../headers/collision_detection.h"

const void objectTerrainCollision(Mesh *terrain, Mesh *object) {
    const float height = getTerrainHeight(terrain, object->pivot, object);
    float height_diff = height - (object->pivot[1] - object->scale);
    if (height_diff >= 0) {
        object->grounded = 1;
        object->falling_time = 0;
    }
    if (object->grounded) {
        Mat4x4 dr = translationMatrix(0, height_diff, 0);

        object->bbox.v = setvecsarrayxm(object->bbox.v, object->bbox.v_indexes, dr);
        object->pivot[1] += height_diff;
    }
}
#include "../headers/logging.h"
const int objectEnvironmentCollision(TerrainInfo *tf, Scene *s, Mesh *obj, vec4f *velocity) {
    if (obj->quadIndex < 0) {
        // fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", obj->quadIndex);
        return 0;
    }

    obj->dm = getDimensionsLimits(obj->bbox.v, obj->bbox.v_indexes);

    vec4f tnear, tfar;
    int f_nx, f_ny, f_nz, f_fx, f_fy, f_fz;

    Mesh *cache;
    const int num_of_members = tf->quads[obj->quadIndex].members_indexes;

    for (int i = 0; i < num_of_members; i++) {

        cache = tf->quads[obj->quadIndex].members[i];

        if ( cache->id != obj->id ) {

            cache->dm = getDimensionsLimits(cache->bbox.v, cache->v_indexes);

            vec4i min = __builtin_convertvector((cache->dm.min - (obj->pivot - obj->dm.min)) + 0.5, vec4i);
            vec4i max = __builtin_convertvector((cache->dm.max - (obj->pivot - obj->dm.max)) + 0.5, vec4i);

            tnear = (__builtin_convertvector(min, vec4f) - obj->pivot) / velocity[0];
            tfar =  (__builtin_convertvector(max, vec4f) - obj->pivot) / velocity[0];

            f_nx = tnear[0] == 0 ? 1 : __isnanf(tnear[0]) ? -1 : 0;
            f_ny = tnear[1] == 0 ? 1 : __isnanf(tnear[1]) ? -1 : 0;
            f_nz = tnear[2] == 0 ? 1 : __isnanf(tnear[2]) ? -1 : 0;
            f_fx = tfar[0] == 0 ? 1 : __isnanf(tfar[0]) ? -1 : 0;
            f_fy = tfar[1] == 0 ? 1 : __isnanf(tfar[1]) ? -1 : 0;
            f_fz = tfar[2] == 0 ? 1 : __isnanf(tfar[2]) ? -1 : 0;

            // printf("id: %d\n", s->m[inner_inx].id);

            // printf("f_chk: %d!!!\n", (f_nx | f_ny | f_nz | f_fx | f_fy | f_fz));
            if ( (f_nx | f_ny | f_nz | f_fx | f_fy | f_fz) < 0 ) {
                // loadMaterial(&s->m[inner_inx].material, "jade");
                // printf("Nan value has occured!!!\n");
                continue;
            }

            const int f_sum = f_nx + f_ny + f_nz + f_fx + f_fy + f_fz;
            // printf("f_sum: %d!!!\n", f_sum);
            if (tnear[0] > tfar[0]) swap(&tnear[0], &tfar[0], 4);
            if (tnear[1] > tfar[1]) swap(&tnear[1], &tfar[1], 4);
            if (tnear[2] > tfar[2]) swap(&tnear[2], &tfar[2], 4);

            if (tnear[0] > tfar[2] || tnear[2] > tfar[0]) {
                // printf("(tnear[0] > tfar[2] || tnear[2] > tfar[0])\n");
                // loadMaterial(&s->m[inner_inx].material, "jade");
                continue;
            }

            float t_near = tnear[0] > tnear[2] ? tnear[0] : tnear[2];
            float t_far = tfar[0] < tfar[2] ? tfar[0] : tfar[2];

            /* ##################### Y ############################ */
            if (t_near > tfar[1] || tnear[1] > t_far) {
                // printf("(t_near > tfar[1] || tnear[1] > t_far)\n");
                // getc(stdin);
                // loadMaterial(&s->m[inner_inx].material, "jade");
                continue;
            }

            if (tnear[1] > t_near) {
                t_near = tnear[1];
                // printf("(tnear[1] > t_near)\n");
                // getc(stdin);
            }
            if (tfar[1] < t_far) {
                t_far = tfar[1];
                // printf("(tfar[1] < t_far)\n");
                // getc(stdin);
            }
            /* ##################### Y ############################ */          

            if ( ((t_far < 0) || (t_near < 0)) || (t_near > 1.f) ) { 
                // printf("(t_far < 0 || t_near < 0) || (t_near > 1.f)\n");
                // loadMaterial(&s->m[inner_inx].material, "jade");
                continue;
            }

            // printf("minx: %d    miny: %d    minz: %d\n", minx, miny, minz);
            // printf("maxx: %d    maxy: %d    maxz: %d\n", maxx, maxy, maxz);

            // printf("FLAGS: f_nx: %d    f_ny: %d    f_nz: %d\n", f_nx, f_ny, f_nz);
            // printf("FLAGS: f_fx: %d    f_fy: %d    f_fz: %d\n", f_fx, f_fy, f_fz);

            // printf("id: %d\n", s->m[inner_inx].id);
            // printf("\ntnear[0]: %f    tnear[1]: %f    tnear[2]: %f\n", tnear[0], tnear[1], tnear[2]);
            // printf("tfar[0]  : %f    tfar[1]  : %f    tfar[2]  : %f\n", tfar[0], tfar[1], tfar[2]);


            vec4f normal = { 0.f };
            if ( tnear[0] >= tnear[1] && tnear[0] >= tnear[2] ) {
                if ( velocity[0][0] < 0 ) {
                    normal[0] = 1.f;
                    // printf("Right\n");
                } else if ( velocity[0][0] > 0 ) {     
                    normal[0] = -1.f;
                    // printf("Left\n");
                }
            } else if ( tnear[1] >= tnear[0] && tnear[1] >= tnear[2] ) {
                if ( velocity[0][1] < 0 ) {
                    normal[1] = 1.f;
                    // printf("Up\n");
                } else if ( velocity[0][1] > 0 ) {
                    normal[1] = -1.f;
                    // printf("Down\n");
                }
            } else if ( tnear[2] >= tnear[0] && tnear[2] >= tnear[1] ) {
                if ( velocity[0][2] < 0 ) {
                    normal[2] = 1.f;
                    // printf("Back\n");
                } else if ( velocity[0][2] > 0 ) {
                    normal[2] = -1.f;
                    // printf("Front\n");
                }
            } else {
                // printf("EXTRA CASE\n");
                // continue;
            }

            // printf("t_near   :    ----> %f\n", t_near);

            if ( t_near == 0.f && f_sum <= 1 ) {
                printf("Sliding....\n");

                float dot =  dot_product(normal, obj->mvdir);
                obj->mvdir = obj->mvdir - (dot * normal);

                if ( tnear[1] == 0 ) {
                    obj->falling_time = 1.f;
                    velocity[0] = (obj->mvdir * obj->momentum);
                    // printf("tnear[1] == 0.f || tfar[1] == 0.f\n\n");
                } else {
                    // printf("else CASE\n\n");
                    // obj->momentum *= s->m[inner_inx].mass;
                    velocity[0] = (gravity_epicenter * (9.81f * obj->falling_time) * obj->mass) + (obj->mvdir * obj->momentum);
                }
                // loadMaterial(&s->m[inner_inx].material, "pearl");
                return 1;

            } else if ( ((t_near > 0.f) && (t_near <= 1.f)) ) {
                printf("COLLISION!!!\n\n");

                velocity[0] *= t_near;

                Mat4x4 trans = translationMatrix(velocity[0][0], velocity[0][1], velocity[0][2]);

                obj->bbox.v = setvecsarrayxm(obj->bbox.v, obj->bbox.v_indexes, trans);

                obj->pivot += velocity[0];

                obj->momentum *= cache->mass;
                float dot =  dot_product(normal, obj->mvdir);
                obj->mvdir = obj->mvdir - (dot * normal);
                velocity[0] = (obj->mvdir * obj->momentum);

                // loadMaterial(&s->m[inner_inx].material, "gold");
                return 1;
            }
        }
    }
    return 0;
}
const int rotationCollision(TerrainInfo *tf, Scene *s, Mesh *obj) {
    if (obj->quadIndex < 0) {
        // fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", obj->quadIndex);
        return 0;
    }

    obj->dm = getDimensionsLimits(obj->bbox.v, obj->bbox.v_indexes);

    vec4f tnear, tfar;
    int f_nx, f_ny, f_nz, f_fx, f_fy, f_fz;

    Mesh *cache;
    const int num_of_members = tf->quads[obj->quadIndex].members_indexes;

    for (int i = 0; i < num_of_members; i++) {

        cache = tf->quads[obj->quadIndex].members[i];

        if ( cache->id != obj->id ) {

            cache->dm = getDimensionsLimits(cache->bbox.v, cache->bbox.v_indexes);

            vec4i min = __builtin_convertvector((cache->dm.min - (obj->pivot - obj->dm.min)) + 0.5, vec4i);
            vec4i max = __builtin_convertvector((cache->dm.max - (obj->pivot - obj->dm.max)) + 0.5, vec4i);

            tnear = (__builtin_convertvector(min, vec4f) - obj->pivot);
            tfar =  (__builtin_convertvector(max, vec4f) - obj->pivot);

            f_nx = tnear[0] == 0 ? 1 : __isnanf(tnear[0]) ? -1 : 0;
            f_ny = tnear[1] == 0 ? 1 : __isnanf(tnear[1]) ? -1 : 0;
            f_nz = tnear[2] == 0 ? 1 : __isnanf(tnear[2]) ? -1 : 0;
            f_fx = tfar[0] == 0 ? 1 : __isnanf(tfar[0]) ? -1 : 0;
            f_fy = tfar[1] == 0 ? 1 : __isnanf(tfar[1]) ? -1 : 0;
            f_fz = tfar[2] == 0 ? 1 : __isnanf(tfar[2]) ? -1 : 0;

            // printf("FLAGS: f_nx: %d    f_ny: %d    f_nz: %d\n", f_nx, f_ny, f_nz);
            // printf("FLAGS: f_fx: %d    f_fy: %d    f_fz: %d\n", f_fx, f_fy, f_fz);

            // printf("id: %d\n", s->m[inner_inx].id);
            // printf("\ntnear[0]: %f    tnear[1]: %f    tnear[2]: %f\n", tnear[0], tnear[1], tnear[2]);
            // printf("tfar[0]  : %f    tfar[1]  : %f    tfar[2]  : %f\n", tfar[0], tfar[1], tfar[2]);

            if ( (f_nx | f_ny | f_nz | f_fx | f_fy | f_fz) < 0 ) {
                continue;
            }

            /* This or we have to devide tnear and tfar with 0 for penetration detection to work. */
            const int f_sum = f_nx + f_ny + f_nz + f_fx + f_fy + f_fz;

            if (tnear[0] > tfar[0]) swap(&tnear[0], &tfar[0], 4);
            if (tnear[1] > tfar[1]) swap(&tnear[1], &tfar[1], 4);
            if (tnear[2] > tfar[2]) swap(&tnear[2], &tfar[2], 4);

            if (tnear[0] > tfar[2] || tnear[2] > tfar[0]) {
                continue;
            }

            float t_near = tnear[0] > tnear[2] ? tnear[0] : tnear[2];
            float t_far = tfar[0] < tfar[2] ? tfar[0] : tfar[2];

            /* ##################### Y ############################ */
            if (t_near > tfar[1] || tnear[1] > t_far) {
                continue;
            }

            if (tnear[1] > t_near) {
                t_near = tnear[1];
            }
            if (tfar[1] < t_far) {
                t_far = tfar[1];
            }
            /* ##################### Y ############################ */          

            if ( (t_far < 0) || (t_near > 1.f) ) { 
                continue;
            }

            if ( (t_near < 0) && (f_sum == 0) ) {
                printf("PENETRATION!!! %f\n\n", t_near);
                // printf("before:  ");
                // logQuat(obj->r);
                Quat rq = conjugateQuat(obj->r);
                // printf("after :  ");
                // logQuat(obj->r);
                // printf("\n");
                Mat4x4 rm = MatfromQuat(rq, obj->pivot);
                obj->Q = multiplyQuats(obj->Q, rq);

                obj->bbox.v = setvecsarrayxm(obj->bbox.v, obj->bbox.v_indexes, rm);
                obj->r = unitQuat();
                return 1;
            }
        }
    }
    return 0;
}
const void sortObjectCollisions(TerrainInfo *tf, Scene *s, Mesh *obj, vec4f *velocity) {
    if (obj->quadIndex < 0) {
        // fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", obj->quadIndex);
        return;
    }

    obj->dm = getDimensionsLimits(obj->bbox.v, obj->bbox.v_indexes);

    vec4f tnear, tfar;
    int f_nx, f_ny, f_nz, f_fx, f_fy, f_fz;

    Mesh *cache_1;
    const int num_of_members = tf->quads[obj->quadIndex].members_indexes;

    for (int i = 0; i < num_of_members; i++) {

        cache_1 = tf->quads[obj->quadIndex].members[i];
        cache_1->collision_t = 1000000.f;

        if ( cache_1->id != obj->id ) {
            cache_1->dm = getDimensionsLimits(cache_1->bbox.v, cache_1->bbox.v_indexes);

            vec4i min = __builtin_convertvector((cache_1->dm.min - (obj->pivot - obj->dm.min)) + 0.5, vec4i);
            vec4i max = __builtin_convertvector((cache_1->dm.max - (obj->pivot - obj->dm.max)) + 0.5, vec4i);

            tnear = (__builtin_convertvector(min, vec4f) - obj->pivot) / velocity[0];
            tfar =  (__builtin_convertvector(max, vec4f) - obj->pivot) / velocity[0];

            f_nx = tnear[0] == 0 ? 1 : __isnanf(tnear[0]) ? -1 : 0;
            f_ny = tnear[1] == 0 ? 1 : __isnanf(tnear[1]) ? -1 : 0;
            f_nz = tnear[2] == 0 ? 1 : __isnanf(tnear[2]) ? -1 : 0;
            f_fx = tfar[0] == 0 ? 1 : __isnanf(tfar[0]) ? -1 : 0;
            f_fy = tfar[1] == 0 ? 1 : __isnanf(tfar[1]) ? -1 : 0;
            f_fz = tfar[2] == 0 ? 1 : __isnanf(tfar[2]) ? -1 : 0;

            if ( (f_nx | f_ny | f_nz | f_fx | f_fy | f_fz) < 0 ) {
                continue;
            }

            if (tnear[0] > tfar[0]) swap(&tnear[0], &tfar[0], 4);
            if (tnear[1] > tfar[1]) swap(&tnear[1], &tfar[1], 4);
            if (tnear[2] > tfar[2]) swap(&tnear[2], &tfar[2], 4);

            if (tnear[0] > tfar[2] || tnear[2] > tfar[0]) {
                continue;
            }

            float t_near = tnear[0] > tnear[2] ? tnear[0] : tnear[2];
            float t_far = tfar[0] < tfar[2] ? tfar[0] : tfar[2];

            if (t_near > tfar[1] || tnear[1] > t_far) {
                continue;
            }

            if (tnear[1] > t_near) {
                t_near = tnear[1];
            }
            if (tfar[1] < t_far) {
                t_far = tfar[1];
            }

            if (t_far < 0) { 
                continue;
            }

            cache_1->collision_t = t_near;
        }
    }
    Mesh *cache_2;
    for (int i = 0; i < num_of_members; i++) {
        cache_1 = tf->quads[obj->quadIndex].members[i];
        for (int j = 0; j < num_of_members; j++) {
            cache_2 = tf->quads[obj->quadIndex].members[j];
            if (cache_1->id != obj->id) {
                if (cache_1->collision_t < cache_2->collision_t) {
                    swap(&tf->quads[obj->quadIndex].members[i], &tf->quads[obj->quadIndex].members[j], 4);
                }
            }
        }
    }
}


