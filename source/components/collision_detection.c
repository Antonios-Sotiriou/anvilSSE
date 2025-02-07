#include "../../headers/components/collision_detection.h"

/* Return signed shortest distance from point to plane, plane normal must be normalised. */
const float dist(vec4f plane_p, vec4f plane_n, vec4f v) {
    vec4f r = plane_n * v;
    return ( (r[0] + r[1] + r[2]) - dot_product(plane_n, plane_p) );
}
const vec4f plane_intersect(vec4f plane_p, vec4f plane_n, vec4f line_start, vec4f line_end, float *t) {

    vec4f u = line_end - line_start;
    float dot = dot_product(plane_n, u);
    vec4f w = line_start - plane_p;
    *t = -dot_product(plane_n, w) / dot;
    if (fabs(*t) <= 0.000062)
        *t = 0.f;
    return line_start + (u * *t);
}
float sweptDoubleTri(face sf, face mf, vec4f velocity, vec4f *n) {
    float d0, d1, t = __INT_MAX__;
    vec4f stable_n = norm_vec(triangle_cp(sf));
    vec4f moving_n = norm_vec(triangle_cp(mf));
    float dot1 = dot_product(stable_n, moving_n);

    // displayFace(&mf, worldMatrix);
    // displayFace(&sf, worldMatrix);

    // ############################################## VECTORS COLLIDE ##########################################################
    // vec4f s_edges[3] = {
    //     sf.v[0] - sf.v[1],
    //     sf.v[1] - sf.v[2],
    //     sf.v[2] - sf.v[0]
    // };
    // int m_collide = 0;
    // for (int i = 0; i < 3; i++) {
    //     d0 = dist(sf.v[0], stable_n, mf.v[i]);
    //     d1 = dist(sf.v[0], stable_n, mf.v[i] + velocity);
    //     float temp;
    //     if (d1 <= 0 && d0 >= 0) {
    //         // drawLine(mf.v[i], mf.v[i] + (velocity * 1000), worldMatrix);

    //         vec4f temp_p = plane_intersect(sf.v[0], stable_n, mf.v[i], mf.v[i] + velocity, &temp);
    //         if ( dot_product(stable_n, cross_product(s_edges[0], sf.v[0] - temp_p)) <= 0 ) continue;
    //         if ( dot_product(stable_n, cross_product(s_edges[1], sf.v[1] - temp_p)) <= 0 ) continue;
    //         if ( dot_product(stable_n, cross_product(s_edges[2], sf.v[2] - temp_p)) <= 0 ) continue;

    //         if ( temp < t && (temp >= 0 && temp <= 1) ) {
    //             t = temp;
    //             *n = stable_n;
    //             drawLine(temp_p, temp_p + (velocity * 1000), worldMatrix);
    //             displayPoint(temp_p, worldMatrix, 0xff0000);
    //             m_collide++;
    //         }
    //     }
    // }
    // if (m_collide) {
    //     printf("Vector m collision\n");
    //     return t;
    // }
    // int s_collide = 0;
    // if (!m_collide) {
    //     vec4f m_edges[3] = {
    //         mf.v[0] - mf.v[1],
    //         mf.v[1] - mf.v[2],
    //         mf.v[2] - mf.v[0]
    //     };
    //     for (int i = 0; i < 3; i++) {
    //         d0 = dist(mf.v[0], moving_n, sf.v[i]);
    //         d1 = dist(mf.v[0], moving_n, sf.v[i] - velocity);
    //         float temp;
    //         if (d1 <= 0 && d0 >= 0) {
    //             // drawLine(sf.v[i], sf.v[i] - (velocity * 1000), worldMatrix);

    //             vec4f temp_p = plane_intersect(mf.v[0], moving_n, sf.v[i], sf.v[i] - velocity, &temp);
    //             if ( dot_product(moving_n, cross_product(m_edges[0], mf.v[0] - temp_p)) <= 0 ) continue;
    //             if ( dot_product(moving_n, cross_product(m_edges[1], mf.v[1] - temp_p)) <= 0 ) continue;
    //             if ( dot_product(moving_n, cross_product(m_edges[2], mf.v[2] - temp_p)) <= 0 ) continue;

    //             if ( temp < t && (temp >= 0 && temp <= 1) ) {
    //                 t = temp;
    //                 *n = moving_n;
    //                 drawLine(temp_p, temp_p - (velocity * 1000), worldMatrix);
    //                 displayPoint(temp_p, worldMatrix, 0x00ff00);
    //                 s_collide++;
    //             }
    //         }
    //     }
    // }
    // if (s_collide) {
    //     printf("Vector s collision\n");
    //     return t;
    // }
    // ############################################## VECTORS COLLIDE ##########################################################
    sf.v[0] = round_vec4f(sf.v[0]);
    sf.v[1] = round_vec4f(sf.v[1]);
    sf.v[2] = round_vec4f(sf.v[2]);
    mf.v[0] = round_vec4f(mf.v[0]);
    mf.v[1] = round_vec4f(mf.v[1]);
    mf.v[2] = round_vec4f(mf.v[2]);

    vec4f line_start[3] = { sf.v[1], sf.v[2], sf.v[0] };
    vec4f line_end[3] = { sf.v[0], sf.v[1], sf.v[2] };

    vec4f m_edges[3] = {
        mf.v[1] - mf.v[0],
        mf.v[2] - mf.v[1],
        mf.v[0] - mf.v[2]
    };
    vec4f m1_edges[4] = {
        mf.v[0] - mf.v[2],
        mf.v[2] - (mf.v[2] + velocity * 1000),
        (mf.v[2] + velocity * 1000) - (mf.v[0] + velocity * 1000),
        (mf.v[0] + velocity * 1000) - mf.v[0]
    };

    printf("\x1b[H\x1b[J");
    // logFace(sf, 1, 0, 0);
    // logFace(mf, 1, 0, 0); 

    float temp, test;
    for (int x = 0; x < 3; x++) {
        vec4f normal = norm_vec(cross_product(m_edges[x], mf.v[x] * velocity));
        // drawLine(mf.v[x] , mf.v[x] + (normal * 1000), worldMatrix);

        for (int y = 0; y < 3; y++) {

                vec4f r = plane_intersect(mf.v[x], normal, line_start[y], line_end[y], &test);
                if (test >= 0 && test <= 1) {

                    // displayPoint(r, worldMatrix, 0x00ff28);
                    vec4f test1 = plane_intersect(mf.v[x], moving_n, r, r - velocity, &temp);
                    float e1, e2, e3;
                    if ( e1 = dot_product(moving_n, cross_product(m_edges[0], mf.v[0] - test1)) > 0) continue;
                    if ( e2 = dot_product(moving_n, cross_product(m_edges[1], mf.v[1] - test1)) > 0) continue;
                    if ( e3 = dot_product(moving_n, cross_product(m_edges[2], mf.v[2] - test1)) > 0) continue;

                    printf("e1: %f    e2: %f    e3: %f\n", e1, e2, e3);
                    drawLine(r, test1, worldMatrix);
                    // if ( temp < t && (temp >= 0 && temp <= 1) ) {
                    t = temp;
                    //     *n = stable_n;
                    //     // drawLine(r, test1, worldMatrix);
                    //     displayPoint(r, worldMatrix, 0x00ff28);
                    //     printf("edge collision: %f\n", t);
                    // }
                }
        }
    }
    return t;
}


const void terrainCollision(Mesh *terrain, Mesh *obj) {
    TerrainPointInfo tp = getTerrainPointData(terrain, obj->cd.v[P], obj);
    float height_diff = tp.pos[1] - (obj->cd.v[P][1] - obj->scale);
    if ( height_diff >= 0 ) {
        obj->grounded = 1;
        obj->falling_time = 0;
    }
    if ( obj->grounded ) {
        Mat4x4 tm = translationMatrix(0, height_diff, 0);

        setvecsarrayxm(obj->cd.v, 4, tm);
        setvecsarrayxm(obj->bbox.v, obj->bbox.v_indexes, tm);
        setfacesarrayxm(obj->bbox.f, obj->bbox.f_indexes, tm);

        float dot =  dot_product(tp.normal, obj->velocity);
        obj->velocity = (obj->velocity - (dot * tp.normal)) * terrain->fr_coef;
    }
}
const void terrainHeightDifference(Mesh *terrain, Mesh *obj) {
    vec4f next_pos = obj->cd.v[P] + obj->velocity + (obj->mvdir * obj->scale);
    // displayPoint(next_pos, worldMatrix, 0xffccaa);
    TerrainPointInfo tp_0 = getTerrainPointData(terrain, obj->cd.v[0], obj);
    TerrainPointInfo tp_1 = getTerrainPointData(terrain, next_pos, obj);

    float height_0 = tp_0.pos[1] - (obj->cd.v[P][1] - obj->scale);
    float height_1 = tp_1.pos[1] - (next_pos[1] - obj->scale);

    if ( (obj->grounded) && (height_1 - height_0 > 50) ) {
        obj->velocity -= obj->velocity;
    }
}
const int aabbCollision(TerrainInfo *ti, Scene *s, Mesh *obj) {

    if ( obj->quadIndex < 0 ) {
        fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", obj->quadIndex);
        return 0;
    }

    obj->dm = getDimensionsLimits(obj->bbox.v, obj->bbox.v_indexes);
    vec4f tnear, tfar;
    int f_nx, f_ny, f_nz, f_fx, f_fy, f_fz;

    Mesh *cache;
    const int num_of_members = ti->quads[obj->quadIndex].members_indexes;

    for (int i = 0; i < num_of_members; i++) {

        int pk = ti->quads[obj->quadIndex].members[i];
        cache = &scene.m[pk];

        if ( pk != obj->id ) {

            cache->dm = getDimensionsLimits(cache->bbox.v, cache->bbox.v_indexes);

            vec4i min = __builtin_convertvector((cache->dm.min) - (obj->cd.v[P] - (obj->dm.min)) + 0.5f, vec4i);
            vec4i max = __builtin_convertvector((cache->dm.max) - (obj->cd.v[P] - (obj->dm.max)) + 0.5f, vec4i);

            tnear = (__builtin_convertvector(min, vec4f) - obj->cd.v[P]) / obj->velocity;
            tfar =  (__builtin_convertvector(max, vec4f) - obj->cd.v[P]) / obj->velocity;

            f_nx = tnear[0] == 0 ? 1 : __isnanf(tnear[0]) ? -1 : 0;
            f_ny = tnear[1] == 0 ? 1 : __isnanf(tnear[1]) ? -1 : 0;
            f_nz = tnear[2] == 0 ? 1 : __isnanf(tnear[2]) ? -1 : 0;
            f_fx = tfar[0] == 0 ? 1 : __isnanf(tfar[0]) ? -1 : 0;
            f_fy = tfar[1] == 0 ? 1 : __isnanf(tfar[1]) ? -1 : 0;
            f_fz = tfar[2] == 0 ? 1 : __isnanf(tfar[2]) ? -1 : 0;

            if ( (f_nx | f_ny | f_nz | f_fx | f_fy | f_fz) < 0 )
                continue;

            if ( tnear[0] > tfar[0] ) swap(&tnear[0], &tfar[0], 4);
            if ( tnear[1] > tfar[1] ) swap(&tnear[1], &tfar[1], 4);
            if ( tnear[2] > tfar[2] ) swap(&tnear[2], &tfar[2], 4);

            if ( tnear[0] > tfar[2] || tnear[2] > tfar[0] )
                continue;

            float t_near = tnear[0] > tnear[2] ? tnear[0] : tnear[2];
            float t_far = tfar[0] < tfar[2] ? tfar[0] : tfar[2];

            /* ##################### Y ############################ */
            if ( t_near > tfar[1] || tnear[1] > t_far )
                continue;

            if ( tnear[1] > t_near )
                t_near = tnear[1];
            if ( tfar[1] < t_far )
                t_far = tfar[1];
            /* ##################### Y ############################ */

            if ( ((t_far < 0) || (t_near < 0)) || (t_near > 1.f) )
                continue;

            vec4f normal = { 0.f };
            if ( tnear[0] >= tnear[1] && tnear[0] >= tnear[2] ) {
                if ( obj->velocity[0] < 0 ) {
                    normal[0] = 1.f;
                } else if ( obj->velocity[0] > 0 ) {     
                    normal[0] = -1.f;
                }
            } else if ( tnear[1] >= tnear[0] && tnear[1] >= tnear[2] ) {
                if ( obj->velocity[1] < 0 ) {
                    normal[1] = 1.f;
                } else if ( obj->velocity[1] > 0 ) {
                    normal[1] = -1.f;
                }
            } else if ( tnear[2] >= tnear[0] && tnear[2] >= tnear[1] ) {
                if ( obj->velocity[2] < 0 ) {
                    normal[2] = 1.f;
                } else if ( obj->velocity[2] > 0 ) {
                    normal[2] = -1.f;
                }
            }

            if ( t_near == 0.f ) {
                printf("Sliding.... %f\n", t_near);

                float dot = dot_product(normal, obj->velocity);
                obj->velocity = obj->velocity - (dot * normal);
                return 1;

            } else if ( ((t_near > 0.f) && (t_near <= 1.f)) ) {
                printf("COLLISION  ######################### : %f    mesh.id: %d\n", t_near, cache->id);

                obj->velocity *= t_near;

                Mat4x4 trans = translationMatrix(obj->velocity[0], obj->velocity[1], obj->velocity[2]);

                setvecsarrayxm(obj->cd.v, 4, trans);
                setvecsarrayxm(obj->bbox.v, obj->bbox.v_indexes, trans);
                setfacesarrayxm(obj->bbox.f, obj->bbox.f_indexes, trans);

                float dot = dot_product(normal, obj->velocity);
                obj->velocity = obj->velocity - (dot * normal);

                return 1;
            }
        }
    }
    return 0;
}
const int obbCollision(Mesh *m) {
    const int pks_num = scene.t.quads[m->quadIndex].members_indexes;
    // printf("\x1b[H\x1b[J");

    float t = __INT_MAX__;
    vec4f plane_n;
    // for (int i = 0; i < pks_num; i++) {

    //     int pk = scene.t.quads[m->quadIndex].members[i];
    //     if (pk != m->id && pk != 6) {

            for (int x = 0; x < m->bbox.f_indexes; x++) {
                plane_n = norm_vec(triangle_cp(m->bbox.f[x]));
                /* Draw the face normals */
                // drawLine(m->cd.v[0], m->cd.v[0] + (plane_n * 1000), worldMatrix);
                if (dot_product(m->velocity, plane_n) > 0) {
                    displayFace(&m->bbox.f[x], worldMatrix);

                    for (int y = 0; y < scene.m[2].bbox.f_indexes; y++) {
                        // displayFace(&scene.m[2].bbox.f[y], worldMatrix);
                        plane_n = norm_vec(triangle_cp(scene.m[2].bbox.f[y]));
                        /* Draw the face normals */
                        // drawLine(scene.m[2].cd.v[0], scene.m[2].cd.v[0] + (plane_n * 1000), worldMatrix);
                        float d0 = dist(scene.m[2].bbox.f[y].v[0], plane_n, m->cd.v[0]);
                        if (d0 >= 0) {
                            if (dot_product(m->velocity, plane_n) < 0) {
                                displayFace(&scene.m[2].bbox.f[y], worldMatrix);

                                float temp = sweptDoubleTri(scene.m[2].bbox.f[y], m->bbox.f[x], m->velocity, &plane_n);
                                if (temp < t)
                                    t = temp;
                            }
                        }
                    }
                }
            }
            drawLine(m->cd.v[0], m->cd.v[0] + (plane_n * 1000), worldMatrix);
            if (t == 0.f) {
                printf("Sliding...: %f\n", t);
                float dot =  dot_product(plane_n, m->velocity);
                m->velocity = m->velocity - (dot * plane_n);
                if (plane_n[1] == 1)
                    m->falling_time = 0;

                return 1;
            } else if (t > 0.f && t <= 1.f) {
                printf("Collision: %f\n", t);
                m->velocity *= t;
                Mat4x4 trans = translationMatrix(m->velocity[0], m->velocity[1], m->velocity[2]);
                setvecsarrayxm(m->cd.v, 4, trans);
                setvecsarrayxm(m->bbox.v, m->bbox.v_indexes, trans);
                setfacesarrayxm(m->bbox.f, m->bbox.f_indexes, trans);

                float dot =  dot_product(plane_n, m->velocity);
                m->velocity = m->velocity - (dot * plane_n);
                return 1;
            }
        // }
    // }
    return 0;
}
const int rotationCollision(TerrainInfo *ti, Scene *s, Mesh *obj) {

    if ( obj->quadIndex < 0 ) {
        fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. rotationCollision().\n", obj->quadIndex);
        return 0;
    }

    Mat4x4 tm = MatfromQuat(obj->r, obj->cd.v[P]);
    vec4f *arr = vecsarrayxm(obj->bbox.v, obj->bbox.v_indexes, tm);

    DimensionsLimits dm = getDimensionsLimits(arr, obj->bbox.v_indexes);

    vec4f tnear, tfar;
    int f_nx, f_ny, f_nz, f_fx, f_fy, f_fz;

    Mesh *cache;
    const int num_of_members = ti->quads[obj->quadIndex].members_indexes;

    for (int i = 0; i < num_of_members; i++) {

        int pk = ti->quads[obj->quadIndex].members[i];
        cache = &scene.m[pk];

        if ( pk != obj->id ) {

            cache->dm = getDimensionsLimits(cache->bbox.v, cache->bbox.v_indexes);

            vec4i min = __builtin_convertvector((cache->dm.min - (obj->cd.v[P] - dm.min)) + 0.5, vec4i);
            vec4i max = __builtin_convertvector((cache->dm.max - (obj->cd.v[P] - dm.max)) + 0.5, vec4i);

            tnear = (__builtin_convertvector(min, vec4f) - obj->cd.v[P]);
            tfar =  (__builtin_convertvector(max, vec4f) - obj->cd.v[P]);

            f_nx = tnear[0] == 0 ? 1 : __isnanf(tnear[0]) ? -1 : 0;
            f_ny = tnear[1] == 0 ? 1 : __isnanf(tnear[1]) ? -1 : 0;
            f_nz = tnear[2] == 0 ? 1 : __isnanf(tnear[2]) ? -1 : 0;
            f_fx = tfar[0] == 0 ? 1 : __isnanf(tfar[0]) ? -1 : 0;
            f_fy = tfar[1] == 0 ? 1 : __isnanf(tfar[1]) ? -1 : 0;
            f_fz = tfar[2] == 0 ? 1 : __isnanf(tfar[2]) ? -1 : 0;

            if ( (f_nx | f_ny | f_nz | f_fx | f_fy | f_fz) < 0 )
                continue;

            if ( tnear[0] > tfar[0] ) swap(&tnear[0], &tfar[0], 4);
            if ( tnear[1] > tfar[1] ) swap(&tnear[1], &tfar[1], 4);
            if ( tnear[2] > tfar[2] ) swap(&tnear[2], &tfar[2], 4);

            if ( tnear[0] > tfar[2] || tnear[2] > tfar[0] )
                continue;

            float t_near = tnear[0] > tnear[2] ? tnear[0] : tnear[2];
            float t_far = tfar[0] < tfar[2] ? tfar[0] : tfar[2];

            /* ##################### Y ############################ */
            if ( t_near > tfar[1] || tnear[1] > t_far )
                continue;

            if ( tnear[1] > t_near )
                t_near = tnear[1];
            if ( tfar[1] < t_far )
                t_far = tfar[1];
            /* ##################### Y ############################ */

            if ( (t_far <= 0) || (t_near > 1.f) ) 
                continue;

            if ( t_near < 0 ) {
                printf("PENETRATION: %f    mesh.id: %d\n", t_near, cache->id);

                free(arr);
                return 1;
            }
        }
    }

    obj->Q = multiplyQuats(obj->Q, obj->r);
    setvecsarrayxm(obj->cd.v, 4, tm);
    memcpy(obj->bbox.v, arr, obj->bbox.v_indexes * 16);
    setfacesarrayxm(obj->bbox.f, obj->bbox.f_indexes, tm);
    free(arr);

    return 0;
}
const void sortCollisions(TerrainInfo *ti, Scene *s, Mesh *obj) {
    if ( obj->quadIndex < 0 ) {
        fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. sortObjectCollision().\n", obj->quadIndex);
        return;
    }

    obj->dm = getDimensionsLimits(obj->bbox.v, obj->bbox.v_indexes);

    vec4f tnear, tfar;
    int f_nx, f_ny, f_nz, f_fx, f_fy, f_fz;

    Mesh *cache_1;
    const int num_of_members = ti->quads[obj->quadIndex].members_indexes;

    for (int i = 0; i < num_of_members; i++) {

        int pk = ti->quads[obj->quadIndex].members[i];
        cache_1 = &scene.m[pk];
        cache_1->collision_t = 1000000.f;

        if ( pk != obj->id ) {
            cache_1->dm = getDimensionsLimits(cache_1->bbox.v, cache_1->bbox.v_indexes);

            vec4i min = __builtin_convertvector((cache_1->dm.min - (obj->cd.v[P] - obj->dm.min)) + 0.5, vec4i);
            vec4i max = __builtin_convertvector((cache_1->dm.max - (obj->cd.v[P] - obj->dm.max)) + 0.5, vec4i);

            tnear = (__builtin_convertvector(min, vec4f) - obj->cd.v[P]) / obj->velocity;
            tfar =  (__builtin_convertvector(max, vec4f) - obj->cd.v[P]) / obj->velocity;

            f_nx = tnear[0] == 0 ? 1 : __isnanf(tnear[0]) ? -1 : 0;
            f_ny = tnear[1] == 0 ? 1 : __isnanf(tnear[1]) ? -1 : 0;
            f_nz = tnear[2] == 0 ? 1 : __isnanf(tnear[2]) ? -1 : 0;
            f_fx = tfar[0] == 0 ? 1 : __isnanf(tfar[0]) ? -1 : 0;
            f_fy = tfar[1] == 0 ? 1 : __isnanf(tfar[1]) ? -1 : 0;
            f_fz = tfar[2] == 0 ? 1 : __isnanf(tfar[2]) ? -1 : 0;

            if ( (f_nx | f_ny | f_nz | f_fx | f_fy | f_fz) < 0 ) {
                continue;
            }

            if ( tnear[0] > tfar[0] ) swap(&tnear[0], &tfar[0], 4);
            if ( tnear[1] > tfar[1] ) swap(&tnear[1], &tfar[1], 4);
            if ( tnear[2] > tfar[2] ) swap(&tnear[2], &tfar[2], 4);

            if ( tnear[0] > tfar[2] || tnear[2] > tfar[0] )
                continue;

            float t_near = tnear[0] > tnear[2] ? tnear[0] : tnear[2];
            float t_far = tfar[0] < tfar[2] ? tfar[0] : tfar[2];

            /* ##################### Y ############################ */
            if ( t_near > tfar[1] || tnear[1] > t_far )
                continue;

            if ( tnear[1] > t_near )
                t_near = tnear[1];
            if ( tfar[1] < t_far )
                t_far = tfar[1];
            /* ##################### Y ############################ */

            if ( t_far < 0 )
                continue;

            cache_1->collision_t = t_near;
        }
    }
    Mesh *cache_2;
    for (int i = 0; i < num_of_members; i++) {
        int pk1 = ti->quads[obj->quadIndex].members[i];
        cache_1 = &scene.m[pk1];
        for (int j = 0; j < num_of_members; j++) {
            int pk2 = ti->quads[obj->quadIndex].members[j];
            cache_2 = &scene.m[pk2];
            if ( cache_1->id != obj->id ) {
                if ( cache_1->collision_t < cache_2->collision_t ) {
                    swap(&ti->quads[obj->quadIndex].members[i], &ti->quads[obj->quadIndex].members[j], 4);
                }
            }
        }
    }
}


