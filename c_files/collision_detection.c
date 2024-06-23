#include "../headers/collision_detection.h"

#include "../headers/logging.h"
extern float DeltaTime, movScalar;
extern vec4f gravity_epicenter;

const void objectTerrainCollision(Mesh *terrain, Mesh *object) {
    const float height = getTerrainHeight(terrain, object->pivot, object);
    float height_diff = height - (object->pivot[1] - object->scale);
    if (height_diff >= 0) {
        object->grounded = 1;
        object->falling_time = 0;
    }
    if (object->grounded) {
        Mat4x4 dr = translationMatrix(0, height_diff, 0);
        // object->v = setvecsarrayxm(object->v, object->v_indexes, dr);
        // object->n = setvecsarrayxm(object->n, object->n_indexes, dr);

        object->bbox.v = setvecsarrayxm(object->bbox.v, object->bbox.v_indexes, dr);
        object->pivot[1] += height_diff;
    }
}
const int objectEnvironmentCollision(TerrainInfo *tf, Scene *s, Mesh *obj, vec4f *velocity) {
    if (obj->quadIndex < 0) {
        // fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", obj->quadIndex);
        return 0;
    }

    obj->BB = getDimensionsLimits(obj->bbox.v, obj->bbox.v_indexes);

    float tnearx, tneary, tnearz, tfarx, tfary, tfarz;
    int f_nx, f_ny, f_nz, f_fx, f_fy, f_fz;

    const int num_of_members = tf->quads[obj->quadIndex].members_indexes;
    for (int i = 0; i < num_of_members; i++) {

        int inner_inx = tf->quads[obj->quadIndex].members[i];

        if ( s->m[inner_inx].id != obj->id ) {

            s->m[inner_inx].BB = getDimensionsLimits(s->m[inner_inx].bbox.v, s->m[inner_inx].bbox.v_indexes);

            int minx = (s->m[inner_inx].BB.minX - (obj->pivot[0] - obj->BB.minX)) + 0.5;
            int miny = (s->m[inner_inx].BB.minY - (obj->pivot[1] - obj->BB.minY)) + 0.5;
            int minz = (s->m[inner_inx].BB.minZ - (obj->pivot[2] - obj->BB.minZ)) + 0.5;
            int maxx = (s->m[inner_inx].BB.maxX - (obj->pivot[0] - obj->BB.maxX)) + 0.5;
            int maxy = (s->m[inner_inx].BB.maxY - (obj->pivot[1] - obj->BB.maxY)) + 0.5;
            int maxz = (s->m[inner_inx].BB.maxZ - (obj->pivot[2] - obj->BB.maxZ)) + 0.5;

            tnearx = (minx - obj->pivot[0]) / velocity[0][0];
            tneary = (miny - obj->pivot[1]) / velocity[0][1];
            tnearz = (minz - obj->pivot[2]) / velocity[0][2];
            tfarx = (maxx - obj->pivot[0]) / velocity[0][0];
            tfary = (maxy - obj->pivot[1]) / velocity[0][1];
            tfarz = (maxz - obj->pivot[2]) / velocity[0][2];

            f_nx = tnearx == 0 ? 1 : __isnanf(tnearx) ? -1 : 0; 
            f_ny = tneary == 0 ? 1 : __isnanf(tneary) ? -1 : 0; 
            f_nz = tnearz == 0 ? 1 : __isnanf(tnearz) ? -1 : 0; 
            f_fx = tfarx == 0 ? 1 : __isnanf(tfarx) ? -1 : 0;             
            f_fy = tfary == 0 ? 1 : __isnanf(tfary) ? -1 : 0; 
            f_fz = tfarz == 0 ? 1 : __isnanf(tfarz) ? -1 : 0;

            // printf("id: %d\n", s->m[inner_inx].id);

            // printf("Nan value has occured: %d!!!\n", (f_nx | f_ny | f_nz | f_fx | f_fy | f_fz));
            if ( (f_nx | f_ny | f_nz | f_fx | f_fy | f_fz) < 0 ) {
                // loadMaterial(&s->m[inner_inx].material, "jade");
                // printf("Nan value has occured!!!\n");
                continue;
            }

            const int f_sum = f_nx + f_ny + f_nz + f_fx + f_fy + f_fz;

            if (tnearx > tfarx) swap(&tnearx, &tfarx, 4);
            if (tneary > tfary) swap(&tneary, &tfary, 4);
            if (tnearz > tfarz) swap(&tnearz, &tfarz, 4);

            if (tnearx > tfarz || tnearz > tfarx) {
                // printf("(tnearx > tfarz || tnearz > tfarx)\n");
                // loadMaterial(&s->m[inner_inx].material, "jade");
                continue;
            }

            float t_near = tnearx > tnearz ? tnearx : tnearz;
            float t_far = tfarx < tfarz ? tfarx : tfarz;

            /* ##################### Y ############################ */
            if (t_near > tfary || tneary > t_far) {
                // printf("(t_near > tfary || tneary > t_far)\n");
                // getc(stdin);
                // loadMaterial(&s->m[inner_inx].material, "jade");
                continue;
            }

            if (tneary > t_near) {
                t_near = tneary;
                // printf("(tneary > t_near)\n");
                // getc(stdin);
            }
            if (tfary < t_far) {
                t_far = tfary;
                // printf("(tfary < t_far)\n");
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
            // printf("\ntnearx: %f    tneary: %f    tnearz: %f\n", tnearx, tneary, tnearz);
            // printf("tfarx  : %f    tfary  : %f    tfarz  : %f\n", tfarx, tfary, tfarz);


            vec4f normal = { 0.f };
            if ( tnearx >= tneary && tnearx >= tnearz ) {
                if ( velocity[0][0] < 0 ) {
                    normal[0] = 1.f;
                    // printf("Right\n");
                } else if ( velocity[0][0] > 0 ) {     
                    normal[0] = -1.f;
                    // printf("Left\n");
                }
            } else if ( tneary >= tnearx && tneary >= tnearz ) {
                if ( velocity[0][1] < 0 ) {
                    normal[1] = 1.f;
                    // printf("Up\n");
                } else if ( velocity[0][1] > 0 ) {
                    normal[1] = -1.f;
                    // printf("Down\n");
                }
            } else if ( tnearz >= tnearx && tnearz >= tneary ) {
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

                if ( tneary == 0 ) {
                    // obj->falling_time = 0.f;
                    velocity[0] = (obj->mvdir * obj->momentum);
                    // printf("tneary == 0.f || tfary == 0.f\n\n");
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
                // obj->v = setvecsarrayxm(obj->v, obj->v_indexes, trans);
                // obj->n = setvecsarrayxm(obj->n, obj->n_indexes, trans);

                obj->bbox.v = setvecsarrayxm(obj->bbox.v, obj->bbox.v_indexes, trans);

                obj->pivot += velocity[0];

                obj->momentum *= s->m[inner_inx].mass;
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

    obj->BB = getDimensionsLimits(obj->bbox.v, obj->bbox.v_indexes);

    float tnearx, tneary, tnearz, tfarx, tfary, tfarz;
    int f_nx, f_ny, f_nz, f_fx, f_fy, f_fz;

    const int num_of_members = tf->quads[obj->quadIndex].members_indexes;
    for (int i = 0; i < num_of_members; i++) {

        int inner_inx = tf->quads[obj->quadIndex].members[i];

        if ( s->m[inner_inx].id != obj->id ) {

            s->m[inner_inx].BB = getDimensionsLimits(s->m[inner_inx].bbox.v, s->m[inner_inx].bbox.v_indexes);

            int minx = (s->m[inner_inx].BB.minX - (obj->pivot[0] - obj->BB.minX)) + 0.5;
            int miny = (s->m[inner_inx].BB.minY - (obj->pivot[1] - obj->BB.minY)) + 0.5;
            int minz = (s->m[inner_inx].BB.minZ - (obj->pivot[2] - obj->BB.minZ)) + 0.5;
            int maxx = (s->m[inner_inx].BB.maxX - (obj->pivot[0] - obj->BB.maxX)) + 0.5;
            int maxy = (s->m[inner_inx].BB.maxY - (obj->pivot[1] - obj->BB.maxY)) + 0.5;
            int maxz = (s->m[inner_inx].BB.maxZ - (obj->pivot[2] - obj->BB.maxZ)) + 0.5;

            tnearx = (minx - obj->pivot[0]) / 0.f;
            tneary = (miny - obj->pivot[1]) / 0.f;
            tnearz = (minz - obj->pivot[2]) / 0.f;
            tfarx = (maxx - obj->pivot[0]) / 0.f;
            tfary = (maxy - obj->pivot[1]) / 0.f;
            tfarz = (maxz - obj->pivot[2]) / 0.f;

            f_nx = tnearx == 0 ? 1 : __isnanf(tnearx) ? -1 : 0; 
            f_ny = tneary == 0 ? 1 : __isnanf(tneary) ? -1 : 0; 
            f_nz = tnearz == 0 ? 1 : __isnanf(tnearz) ? -1 : 0; 
            f_fx = tfarx == 0 ? 1 : __isnanf(tfarx) ? -1 : 0;             
            f_fy = tfary == 0 ? 1 : __isnanf(tfary) ? -1 : 0; 
            f_fz = tfarz == 0 ? 1 : __isnanf(tfarz) ? -1 : 0;

            // printf("Nan value has occured: %d!!!\n", (f_nx | f_ny | f_nz | f_fx | f_fy | f_fz));
            if ( (f_nx | f_ny | f_nz | f_fx | f_fy | f_fz) < 0 ) {
                // loadMaterial(&s->m[inner_inx].material, "jade");
                // printf("Nan value has occured!!!\n");
                continue;
            }

            const int f_sum = f_nx + f_ny + f_nz + f_fx + f_fy + f_fz;

            if (tnearx > tfarx) swap(&tnearx, &tfarx, 4);
            if (tneary > tfary) swap(&tneary, &tfary, 4);
            if (tnearz > tfarz) swap(&tnearz, &tfarz, 4);

            if (tnearx > tfarz || tnearz > tfarx) {
                // printf("(tnearx > tfarz || tnearz > tfarx)\n");
                // loadMaterial(&s->m[inner_inx].material, "jade");
                continue;
            }

            float t_near = tnearx > tnearz ? tnearx : tnearz;
            float t_far = tfarx < tfarz ? tfarx : tfarz;

            /* ##################### Y ############################ */
            if (t_near > tfary || tneary > t_far) {
                // printf("(t_near > tfary || tneary > t_far)\n");
                // getc(stdin);
                // loadMaterial(&s->m[inner_inx].material, "jade");
                continue;
            }

            if (tneary > t_near) {
                t_near = tneary;
                // printf("(tneary > t_near)\n");
                // getc(stdin);
            }
            if (tfary < t_far) {
                t_far = tfary;
                // printf("(tfary < t_far)\n");
                // getc(stdin);
            }
            /* ##################### Y ############################ */          

            if ( (t_far < 0) || (t_near > 1.f) ) { 
                // printf("(t_far < 0 || t_near < 0) || (t_near > 1.f)\n");
                // loadMaterial(&s->m[inner_inx].material, "jade");
                continue;
            }


            // printf("minx: %d    miny: %d    minz: %d\n", minx, miny, minz);
            // printf("maxx: %d    maxy: %d    maxz: %d\n", maxx, maxy, maxz);

            printf("FLAGS: f_nx: %d    f_ny: %d    f_nz: %d\n", f_nx, f_ny, f_nz);
            printf("FLAGS: f_fx: %d    f_fy: %d    f_fz: %d\n", f_fx, f_fy, f_fz);

            // printf("id: %d\n", s->m[inner_inx].id);
            printf("\ntnearx: %f    tneary: %f    tnearz: %f\n", tnearx, tneary, tnearz);
            printf("tfarx  : %f    tfary  : %f    tfarz  : %f\n", tfarx, tfary, tfarz);

            if ( (t_near < 0) ) {
                printf("PENETRATION!!! %f\n\n", t_near);

                Quat rq = conjugateQuat(obj->r);

                Mat4x4 rm = MatfromQuat(rq, obj->pivot);
                obj->Q = multiplyQuats(obj->Q, rq);

                obj->bbox.v = setvecsarrayxm(obj->bbox.v, obj->bbox.v_indexes, rm);
                obj->r = unitQuat();
                // getc(stdin);
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

    obj->BB = getDimensionsLimits(obj->bbox.v, obj->bbox.v_indexes);

    float tnearx, tneary, tnearz, tfarx, tfary, tfarz;
    int f_nx, f_ny, f_nz, f_fx, f_fy, f_fz;

    const int num_of_members = tf->quads[obj->quadIndex].members_indexes;
    for (int i = 0; i < num_of_members; i++) {

        int inner_inx = tf->quads[obj->quadIndex].members[i];
        s->m[inner_inx].collision_t = 1000000.f;

        if ( s->m[inner_inx].id != obj->id ) {
            s->m[inner_inx].BB = getDimensionsLimits(s->m[inner_inx].bbox.v, s->m[inner_inx].bbox.v_indexes);

            int minx = (s->m[inner_inx].BB.minX - (obj->pivot[0] - obj->BB.minX)) + 0.5;
            int miny = (s->m[inner_inx].BB.minY - (obj->pivot[1] - obj->BB.minY)) + 0.5;
            int minz = (s->m[inner_inx].BB.minZ - (obj->pivot[2] - obj->BB.minZ)) + 0.5;
            int maxx = (s->m[inner_inx].BB.maxX - (obj->pivot[0] - obj->BB.maxX)) + 0.5;
            int maxy = (s->m[inner_inx].BB.maxY - (obj->pivot[1] - obj->BB.maxY)) + 0.5;
            int maxz = (s->m[inner_inx].BB.maxZ - (obj->pivot[2] - obj->BB.maxZ)) + 0.5;

            tnearx = (minx - obj->pivot[0]) / velocity[0][0];
            tneary = (miny - obj->pivot[1]) / velocity[0][1];
            tnearz = (minz - obj->pivot[2]) / velocity[0][2];
            tfarx = (maxx - obj->pivot[0]) / velocity[0][0];
            tfary = (maxy - obj->pivot[1]) / velocity[0][1];
            tfarz = (maxz - obj->pivot[2]) / velocity[0][2];

            f_nx = tnearx == 0 ? 1 : __isnanf(tnearx) ? -1 : 0; 
            f_ny = tneary == 0 ? 1 : __isnanf(tneary) ? -1 : 0; 
            f_nz = tnearz == 0 ? 1 : __isnanf(tnearz) ? -1 : 0; 
            f_fx = tfarx == 0 ? 1 : __isnanf(tfarx) ? -1 : 0;             
            f_fy = tfary == 0 ? 1 : __isnanf(tfary) ? -1 : 0; 
            f_fz = tfarz == 0 ? 1 : __isnanf(tfarz) ? -1 : 0;

            if ( (f_nx | f_ny | f_nz | f_fx | f_fy | f_fz) < 0 ) {
                continue;
            }

            if (tnearx > tfarx) swap(&tnearx, &tfarx, 4);
            if (tneary > tfary) swap(&tneary, &tfary, 4);
            if (tnearz > tfarz) swap(&tnearz, &tfarz, 4);

            if (tnearx > tfarz || tnearz > tfarx) {
                continue;
            }

            float t_near = tnearx > tnearz ? tnearx : tnearz;
            float t_far = tfarx < tfarz ? tfarx : tfarz;

            if (t_near > tfary || tneary > t_far) {
                continue;
            }

            if (tneary > t_near) {
                t_near = tneary;
            }
            if (tfary < t_far) {
                t_far = tfary;
            }

            if (t_far < 0) { 
                continue;
            }

            s->m[inner_inx].collision_t = t_near;
            // printf("id: %d\n", s->m[inner_inx].id);
            // printf("t_near: %f\n", t_near);
        }
    }

    for (int i = 0; i < num_of_members; i++) {
        int inner_inx = tf->quads[obj->quadIndex].members[i];
        for (int j = 0; j < num_of_members; j++) {
            int most_inner_inx = tf->quads[obj->quadIndex].members[j];
            if (s->m[most_inner_inx].id != obj->id) {
                if (s->m[inner_inx].collision_t < s->m[most_inner_inx].collision_t) {
                    swap(&tf->quads[obj->quadIndex].members[i], &tf->quads[obj->quadIndex].members[j], 4);
                }
            }
        }
    }
    // printf("|-------------------------------------|\n\n");
}


