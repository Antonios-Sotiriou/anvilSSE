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
float sweptDoubleTri(face *sf, face *mf, vec4f velocity, vec4f *n) {
    float t = __INT_MAX__;
    vec4f stable_n = norm_vec(triangle_cp(*sf));
    vec4f moving_n = norm_vec(triangle_cp(*mf));

    sf->v[0] = round_vec4f(sf->v[0]);
    sf->v[1] = round_vec4f(sf->v[1]);
    sf->v[2] = round_vec4f(sf->v[2]);
    mf->v[0] = round_vec4f(mf->v[0]);
    mf->v[1] = round_vec4f(mf->v[1]);
    mf->v[2] = round_vec4f(mf->v[2]);

    // ############################################## VECTORS COLLIDE ##########################################################
    // vec4f s_edges[3] = {
    //     sf->v[1] - sf->v[0],
    //     sf->v[2] - sf->v[1],
    //     sf->v[0] - sf->v[2]
    // };
    // int m_collide = 0;
    // for (int i = 0; i < 3; i++) {
    //     float temp;

    //     vec4f temp_p = plane_intersect(sf->v[0], stable_n, mf->v[i], mf->v[i] + velocity, &temp);
    //     if ( dot_product(stable_n, cross_product(s_edges[0], sf->v[0] - temp_p)) > 0 ) continue;
    //     if ( dot_product(stable_n, cross_product(s_edges[1], sf->v[1] - temp_p)) > 0 ) continue;
    //     if ( dot_product(stable_n, cross_product(s_edges[2], sf->v[2] - temp_p)) > 0 ) continue;

    //     if ( temp < t && (temp >= 0 && temp <= 1) ) {
    //         t = temp;
    //         drawLine(temp_p, temp_p + (velocity * 1000), worldMatrix);
    //         displayPoint(temp_p, worldMatrix, 0xff0000);
    //         m_collide++;
    //     }
    // }
    // if (m_collide) {
    //     printf("Vector m collision\n");
    //     return t;
    // }

    // if (m_collide)
    //     return t;

    // int s_collide = 0;
    vec4f m_edges[3] = {
        mf->v[1] - mf->v[0],
        mf->v[2] - mf->v[1],
        mf->v[0] - mf->v[2]
    };
    // for (int i = 0; i < 3; i++) {
    //     float temp;

    //     vec4f temp_p = plane_intersect(mf->v[0], moving_n, sf->v[i], sf->v[i] - velocity, &temp);
    //     if ( dot_product(moving_n, cross_product(m_edges[0], mf->v[0] - temp_p)) > 0 ) continue;
    //     if ( dot_product(moving_n, cross_product(m_edges[1], mf->v[1] - temp_p)) > 0 ) continue;
    //     if ( dot_product(moving_n, cross_product(m_edges[2], mf->v[2] - temp_p)) > 0 ) continue;

    //     if ( temp < t && (temp >= 0 && temp <= 1) ) {
    //         t = temp;
    //         drawLine(temp_p, temp_p - (velocity * 1000), worldMatrix);
    //         displayPoint(temp_p, worldMatrix, 0x00ff00);
    //         s_collide++;
    //     }
    // }
    // if (s_collide) {
    //     printf("Vector s collision\n");
    //     return t;
    // }
    // ############################################## VECTORS COLLIDE ##########################################################
    // printf("\x1b[H\x1b[J");
    // logFace(*sf, 1, 0, 0);
    // printf("\n");
    // logFace(*mf, 1, 0, 0); 

    vec4f line_start[3] = { sf->v[1], sf->v[2], sf->v[0] };
    vec4f line_end[3] = { sf->v[0], sf->v[1], sf->v[2] };

    int ind = 1;
    displayFace(mf, worldMatrix);
    displayFace(sf, worldMatrix);
    // drawLine(sf->v[1], sf->v[2], worldMatrix);
    // drawLine(mf->v[0], mf->v[1], worldMatrix);

    float temp, test;
    for (int x = 0; x < 3; x++) {
        vec4f normal = norm_vec(cross_product(m_edges[x], mf->v[x] * velocity));
        // drawLine(mf->v[x] , mf->v[x] + (normal * 1000), worldMatrix);

        /* Just to draw the edges with velocity #################################################### */
        // if (ind > 2)
        //     ind = 0;
        // vec4f dt = dist(sf->v[0], stable_n, mf->v[x]) < dist(sf->v[0], stable_n, mf->v[ind]) ? mf->v[x] : mf->v[ind];
        // // drawLine(mf->v[ind], mf->v[x], worldMatrix);
        // drawLine(mf->v[ind] + velocity * 100, mf->v[ind], worldMatrix);
        // drawLine(mf->v[x] + velocity * 100, mf->v[ind] + velocity * 100, worldMatrix);
        // drawLine(mf->v[x] + velocity * 100, mf->v[x], worldMatrix);
        // ind++;
        /* Just to draw the edges with velocity #################################################### */

        for (int y = 0; y < 3; y++) {

                vec4f r = plane_intersect(mf->v[x], normal, line_start[y], line_end[y], &test);
                if (test >= 0 && test <= 1) {

                    // displayPoint(r, worldMatrix, 0x00ff28);
                    vec4f test1 = plane_intersect(mf->v[x], moving_n, r, r - velocity, &temp);

                    // if ( dot_product(moving_n, cross_product(m_edges[0], mf->v[0] - test1)) > 0 ) continue;
                    // if ( dot_product(moving_n, cross_product(m_edges[1], mf->v[1] - test1)) > 0 ) continue;
                    // if ( dot_product(moving_n, cross_product(m_edges[2], mf->v[2] - test1)) > 0 ) continue;

                    // printf("e1: %f\n", dot_product(moving_n, cross_product(m_edges[0], mf->v[0] - test1)));
                    if (temp < t && (temp >= 0 && temp <= 1)) {
                        t = temp;
                    }

                    // drawLine(test1, test1 + velocity * 100, worldMatrix);
                    displayPoint(test1, worldMatrix, 0x00ff28);
                    displayPoint(r, worldMatrix, 0xff0000);
                    // displayFace(&mf, worldMatrix);
                    // displayFace(&sf, worldMatrix);
                    // logFace(mf, 1, 0, 0); 
                    // logFace(sf, 1, 0, 0);
                    // logVec4f(test1);
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

            vec4f min = round_vec4f(cache->dm.min - (obj->cd.v[P] - obj->dm.min));
            vec4f max = round_vec4f(cache->dm.max - (obj->cd.v[P] - obj->dm.max));

            tnear = (min - obj->cd.v[P]) / obj->velocity;
            tfar =  (max - obj->cd.v[P]) / obj->velocity;

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
                if (normal[1] == 1)
                    obj->falling_time = 0;

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

    if ( m->quadIndex < 0 ) {
        fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", m->quadIndex);
        return 0;
    }
    const int pks_num = scene.t.quads[m->quadIndex].members_indexes;
    // printf("\x1b[H\x1b[J");

    float t = __INT_MAX__;
    vec4f plane_n, n;
    // for (int i = 0; i < pks_num; i++) {

    //     int pk = scene.t.quads[m->quadIndex].members[i];
    //     if (pk != m->id && pk != 6) {

            // for (int x = 0; x < m->bbox.f_indexes; x++) {
                plane_n = norm_vec(triangle_cp(m->bbox.f[0]));
                /* Draw the face normals */
                // drawLine(m->cd.v[0], m->cd.v[0] + (plane_n * 1000), worldMatrix);
                if (dot_product(m->velocity, plane_n) > 0) {
                    // displayFace(&m->bbox.f[0], worldMatrix);

                    // for (int y = 0; y < scene.m[2].bbox.f_indexes; y++) {
                        // displayFace(&scene.m[2].bbox.f[3], worldMatrix);
                        plane_n = norm_vec(triangle_cp(scene.m[2].bbox.f[3]));
                        /* Draw the face normals */
                        // drawLine(scene.m[2].cd.v[0], scene.m[2].cd.v[0] + (plane_n * 1000), worldMatrix);
                        float d0 = dist(scene.m[2].bbox.f[3].v[0], plane_n, m->cd.v[0]);
                        if (d0 >= 0) {
                            if (dot_product(m->velocity, plane_n) < 0) {
                                // displayFace(&scene.m[2].bbox.f[3], worldMatrix);

                                n = plane_n;
                                float temp = sweptDoubleTri(&scene.m[2].bbox.f[3], &m->bbox.f[0], m->velocity, &n);

                                if (t < __INT_MAX__)
                                    printf("t: %f\n", t);
                                if (temp < t) {
                                    t = temp;
                                }
                            }
                        }
                    // }
                }
            // }
            // if (t < __INT_MAX__)
            //     printf("t: %f\n", t);
            // drawLine(m->cd.v[0], m->cd.v[0] + (n * 1000), worldMatrix);
            if (t == 0.f) {
                printf("Sliding...: %f\n", t);
                float dot =  dot_product(n, m->velocity);
                m->velocity = m->velocity - (dot * n);
                if (n[1] == 1)
                    m->falling_time = 0;

                return 1;
            } else if (t > 0.f && t <= 1.f) {
                printf("Collision: %f\n", t);
                m->velocity *= t;
                Mat4x4 trans = translationMatrix(m->velocity[0], m->velocity[1], m->velocity[2]);
                setvecsarrayxm(m->cd.v, 4, trans);
                setvecsarrayxm(m->bbox.v, m->bbox.v_indexes, trans);
                setfacesarrayxm(m->bbox.f, m->bbox.f_indexes, trans);

                float dot =  dot_product(n, m->velocity);
                m->velocity = m->velocity - (dot * n);
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

            vec4f min = round_vec4f(cache->dm.min - (obj->cd.v[P] - dm.min));
            vec4f max = round_vec4f(cache->dm.max - (obj->cd.v[P] - dm.max));

            tnear = min - obj->cd.v[P];
            tfar =  max - obj->cd.v[P];

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

            vec4f min = round_vec4f(cache_1->dm.min - (obj->cd.v[P] - obj->dm.min));
            vec4f max = round_vec4f(cache_1->dm.max - (obj->cd.v[P] - obj->dm.max));

            tnear = (min - obj->cd.v[P]) / obj->velocity;
            tfar =  (max - obj->cd.v[P]) / obj->velocity;

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


