#include "../../headers/components/collision_detection.h"
float e1, e2, e3;
vec4f r, n;
/* Return signed shortest distance from point to plane, plane normal must be normalised. */
const float dist(vec4f plane_p, vec4f plane_n, vec4f v) {
    vec4f r = plane_n * v;
    return ( (r[0] + r[1] + r[2]) - dot_product(plane_n, plane_p) );
}
const vec4f plane_intersect(vec4f plane_p, vec4f plane_n, vec4f line_start, vec4f line_end, float *t) {
    float plane_d = -dot_product(plane_n, plane_p);
    float ad = dot_product(line_start, plane_n);
    float bd = dot_product(line_end, plane_n);
    *t = ((-plane_d - ad) / (bd - ad));
    vec4f line_ste = line_end - line_start;
    vec4f line_ti = line_ste * (*t);

    return line_start + line_ti;
}
float clippface(vec4f plane_p, vec4f plane_n, face in_f, face pf, vec4f velocity) {
    float d0, d1, t = 1000, test;

    Mat4x4 trans = translationMatrix(velocity[0], velocity[1], velocity[2]);
    face f = facexm(in_f, trans);

    vec4f line_start[3] = { f.v[0], f.v[1], f.v[2] };
    vec4f line_end[3] = { f.v[1], f.v[2], f.v[0] };

    float dot1 = dot_product(plane_n, norm_vec(triangle_cp(in_f)));
    vec4f cp[4];
    cp[0] = norm_vec(velocity);
    cp[1] = norm_vec(cross_product(pf.v[1] - pf.v[0], pf.v[1] * (plane_n * 10.f)));
    cp[2] = norm_vec(cross_product(pf.v[2] - pf.v[1], pf.v[2] * (plane_n * 10.f)));
    cp[3] = norm_vec(cross_product(pf.v[0] - pf.v[2], pf.v[0] * (plane_n * 10.f)));

    // if (dot1 == 0) {
    //     printf("Zero dot\n");
    //     float dot, temp = __INT_MAX__;
    //     float min_m, max_m, min_s, max_s;
    //     for (int x = 0; x < 4; x++) {
    //         min_m = __INT_MAX__, max_m = -__INT_MAX__, min_s = __INT_MAX__, max_s = -__INT_MAX__;
    //         for (int i = 0; i < 3; i++) {
    //             min_m = (dot = dot_product(cp[x], pf.v[i])) < min_m ? dot : min_m;
    //             max_m = dot > max_m ? dot : max_m;

    //             min_s = (dot = dot_product(cp[x], f.v[i])) < min_s ? dot : min_s;
    //             max_s = dot > max_s ? dot : max_s;
    //         }

    //         if ( (min_m >= max_s) || (max_m <= min_s) ) {
    //             return -1;
    //         }
    //     }
    //     float a0 = dist(plane_p, plane_n, in_f.v[0]);
    //     float a1 = dist(plane_p, plane_n, in_f.v[1]);
    //     float a2 = dist(plane_p, plane_n, in_f.v[2]);
    //     int index = 0;
    //     if (a0 < a1 && a0 < a2) {
    //         index = 0;
    //     } else if (a1 < a0 && a1 < a2) {
    //         index = 1;
    //     } else if (a2 < a0 && a2 < a1) {
    //         index = 2;
    //     }
    //     r = plane_intersect(plane_p, plane_n, in_f.v[index], in_f.v[index] + velocity, &t);
    //     // displayPoint(in_f.v[index], worldMatrix, 0xff00a7);
    //     drawLine(in_f.v[index], in_f.v[index] + (velocity * 100.f), worldMatrix);
    //     displayFace(&in_f, worldMatrix);
    //     return t;
    // }

    // if (dot1 == -1.f) {
    //     // printf("Dot is -1\n");
    //     float dot;
    //     float min_m, max_m, min_s, max_s;
    //     for (int x = 0; x < 3; x++) {
    //         min_m = __INT_MAX__, max_m = -__INT_MAX__, min_s = __INT_MAX__, max_s = -__INT_MAX__;
    //         for (int i = 0; i < 3; i++) {
    //             min_m = (dot = dot_product(cp[x], pf.v[i])) < min_m ? dot : min_m;
    //             max_m = dot > max_m ? dot : max_m;

    //             min_s = (dot = dot_product(cp[x], f.v[i])) < min_s ? dot : min_s;
    //             max_s = dot > max_s ? dot : max_s;
    //         }

    //         if ( (min_m >= max_s) || (max_m <= min_s) ) {
    //             return t;
    //         }

    //     }

    //     r = plane_intersect(plane_p, plane_n, in_f.v[0], in_f.v[0] + velocity, &t);
    //     drawLine(r, r + (velocity * 100.f), worldMatrix);
    //     // displayPoint(r, worldMatrix, 0xff00a7);
    //     return t;
    // }
    // if (dot1 > -1 && dot1 < 0) {
        displayFace(&in_f, worldMatrix);
        displayFace(&pf, worldMatrix);
        for (int i = 0; i < 3; i++) {

            d0 = dist(plane_p, plane_n, line_start[i]);
            d1 = dist(plane_p, plane_n, line_end[i]);
            if (d1 != d0 && (d1 <= 0 && d0 >= 0)) {
                r = plane_intersect(plane_p, plane_n, line_start[i], line_end[i], &test);
                e1 = dot_product(plane_n, cross_product(pf.v[0] - pf.v[1], pf.v[0] - r));
                e2 = dot_product(plane_n, cross_product(pf.v[1] - pf.v[2], pf.v[1] - r));
                e3 = dot_product(plane_n, cross_product(pf.v[2] - pf.v[0], pf.v[2] - r));

                float temp;
                if (e1 >= 0 && e2 >= 0 && e3 >= 0) {
                    vec4f temp_p = plane_intersect(in_f.v[0], norm_vec(triangle_cp(in_f)), r + -velocity, r, &temp);

                    if (temp < t) {
                        t = temp;
                        displayPoint(r, worldMatrix, 0x00ff28);
                        displayPoint(temp_p, worldMatrix, 0xff00a7);
                        r = temp_p;
                        // drawLine(r, r + -velocity * 100, worldMatrix);
                    }
                }
            }
        }
    // }
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
const int velocityCollideTest(Mesh *m) {
    Mat4x4 tm = translationMatrix(m->velocity[0], m->velocity[1], m->velocity[2]);
    const int pks_num = scene.t.quads[m->quadIndex].members_indexes;

    for (int i = 0; i < pks_num; i++) {
        int pk = scene.t.quads[m->quadIndex].members[i];
        if (pk != m->id && pk != 6) {
            float min_a = __INT32_MAX__, max_a = -__INT32_MAX__, min_b = __INT32_MAX__, max_b = -__INT32_MAX__, temp;

            for (int j = 0; j < m->bbox.v_indexes; j++) {
                min_a = (temp = dot_product(m->velocity, vecxm(m->bbox.v[j], tm))) < min_a ? temp : min_a;
                max_a = temp > max_a ? temp : max_a;
            }

            for (int k = 0; k < scene.m[pk].bbox.v_indexes; k++) {
                min_b = (temp = dot_product(m->velocity, scene.m[pk].bbox.v[k])) < min_b ? temp : min_b;
                max_b = temp > max_b ? temp : max_b;
            }

            if ( (min_a > max_b) || (max_a < min_b) ) {
                return 0;
            }
        }
    }
    m->velocity -= m->velocity;
    return 1;
}
const int obbCollision(Mesh *m) {
    const int pks_num = scene.t.quads[m->quadIndex].members_indexes;
    // printf("\x1b[H\x1b[J");

    float t = __INT_MAX__;
    vec4f plane_n, plane_p;
    vec4f col_n, col_p;
    face col_fm, col_fs;
    for (int i = 0; i < pks_num; i++) {
        int pk = scene.t.quads[m->quadIndex].members[i];
        if (pk != m->id && pk != 6) {

            for (int j = 0; j < scene.m[pk].bbox.f_indexes; j++) {
                plane_n = norm_vec(triangle_cp(scene.m[pk].bbox.f[j]));
                plane_p = scene.m[pk].cd.v[0] + (plane_n * scene.m[pk].scale);

                for (int d = 0; d < m->bbox.f_indexes; d++) {

                    float d0 = dist(plane_p, plane_n, m->cd.v[0]);
                    if (d0 >= 0 && dot_product(m->velocity, plane_n) < 0) {

                        if (dot_product(m->velocity, plane_n) < 0) {
                            // displayFilledFace(&scene.m[pk].bbox.f[j], worldMatrix);

                            vec4f anti_plane_n = norm_vec(triangle_cp(m->bbox.f[d]));
                            vec4f anti_plane_p = m->cd.v[0] + (anti_plane_n * m->scale);
                            
                            float temp;
                            if (dot_product(m->velocity, anti_plane_n) > 0) {
                                // displayFilledFace(&m->bbox.f[d], worldMatrix);

                                t = clippface(plane_p, plane_n, m->bbox.f[d], scene.m[pk].bbox.f[j], m->velocity);
                                if (t == 0.f) {
                                    printf("Sliding...: %f\n", t);

                                    float dot =  dot_product(plane_n, m->velocity);
                                    m->velocity = m->velocity - (dot * plane_n);
                                    // m->velocity = (gravity_epicenter * (9.81f * (m->falling_time * m->falling_time))) + (m->velocity);

                                    // displayFilledFace(&col_fm, worldMatrix);
                                    // displayFilledFace(&col_fs, worldMatrix);
                                    // displayPoint(col_p, worldMatrix, 0xff00a7);
                                    drawLine(col_p, col_p + (-m->velocity * 100.f), worldMatrix);

                                    return 1;
                                } else if (t > 0.f && t <= 1.f) {
                                    printf("Collision: %f\n", t);

                                    // displayFilledFace(&col_fm, worldMatrix);
                                    // displayFilledFace(&col_fs, worldMatrix);
                                    // displayPoint(col_p, worldMatrix, 0xff00a7);
                                    drawLine(col_p, col_p + (-m->velocity * 100.f), worldMatrix);

                                    m->velocity *= t;
                                    Mat4x4 trans = translationMatrix(m->velocity[0], m->velocity[1], m->velocity[2]);
                                    setvecsarrayxm(m->cd.v, 4, trans);
                                    setvecsarrayxm(m->bbox.v, m->bbox.v_indexes, trans);
                                    setfacesarrayxm(m->bbox.f, m->bbox.f_indexes, trans);

                                    float dot =  dot_product(plane_n, m->velocity);
                                    m->velocity = m->velocity - (dot * plane_n);
                                    return 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
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


