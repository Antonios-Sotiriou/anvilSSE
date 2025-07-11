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
    if (fabs(*t) <= 0.0001)
        *t = 0.f;
    return line_start + (u * *t);
}
float sweptTribbox(face sf, face mf, vec4f velocity) {

    vec4f min_s = sf.v[0], max_s = sf.v[0], min_m = mf.v[0], max_m = mf.v[0];

    for (int i = 1; i < 3; i++) {
        if ( sf.v[i][0] <= min_s[0]) {
            min_s[0] = sf.v[i][0];
        } else if ( sf.v[i][0] > max_s[0]) {
            max_s[0] = sf.v[i][0];
        }
        /* Get min and max y sf->vlues. */
        if ( sf.v[i][1] <= min_s[1]) {
            min_s[1] = sf.v[i][1];
        } else if ( sf.v[i][1] > max_s[1]) {
            max_s[1] = sf.v[i][1];
        }
        /* Get min and max z sf->vlues. */
        if ( sf.v[i][2] <= min_s[2]) {
            min_s[2] = sf.v[i][2];
        } else if ( sf.v[i][2] > max_s[2]) {
            max_s[2] = sf.v[i][2];
        }

        if ( mf.v[i][0] <= min_m[0]) {
            min_m[0] = mf.v[i][0];
        } else if ( mf.v[i][0] > max_m[0]) {
            max_m[0] = mf.v[i][0];
        }
        /* Get min and max y sf->vlues. */
        if ( mf.v[i][1] <= min_m[1]) {
            min_m[1] = mf.v[i][1];
        } else if ( mf.v[i][1] > max_m[1]) {
            max_m[1] = mf.v[i][1];
        }
        /* Get min and max z sf->vlues. */
        if ( mf.v[i][2] <= min_m[2]) {
            min_m[2] = mf.v[i][2];
        } else if ( mf.v[i][2] > max_m[2]) {
            max_m[2] = mf.v[i][2];
        }
    }

    vec4f p = min_m + ((max_m - min_m) * 0.5f);
    vec4f min = round_vec4f(min_s - (p - min_m));
    vec4f max = round_vec4f(max_s - (p - max_m));

    vec4f tnear = (min - p) / velocity;
    vec4f tfar =  (max - p) / velocity;

    if ( tnear[0] > tfar[0] ) swap(&tnear[0], &tfar[0], 4);
    if ( tnear[1] > tfar[1] ) swap(&tnear[1], &tfar[1], 4);
    if ( tnear[2] > tfar[2] ) swap(&tnear[2], &tfar[2], 4);

    if ( tnear[0] > tfar[2] || tnear[2] > tfar[0] )
        return -1;

    float t_near = tnear[0] > tnear[2] ? tnear[0] : tnear[2];
    float t_far = tfar[0] < tfar[2] ? tfar[0] : tfar[2];

    /* ##################### Y ############################ */
    if ( t_near > tfar[1] || tnear[1] > t_far )
        return -1;

    if ( tnear[1] > t_near )
        t_near = tnear[1];
    if ( tfar[1] < t_far )
        t_far = tfar[1];
    /* ##################### Y ############################ */

    if ( ((t_far < 0) || (t_near < 0)) || (t_near > 1.f) )
        return -1;

    if ( ((t_near >= 0.f) && (t_near <= 1.f)) ) {
        printf("COLLISION  ######################### : %f\n", t_near);
        return t_near;
    }
    displayfacedm(sf, worldMatrix);
    displayfacedm(mf, worldMatrix);
}
float sweptTriangleVectors(face sf, face mf, vec4f velocity) {
    float t = __INT_MAX__;
    vec4f stable_n = norm_vec(triangle_cp(sf));
    vec4f moving_n = norm_vec(triangle_cp(mf));

    sf.v[0] = round_vec4f(sf.v[0]);
    sf.v[1] = round_vec4f(sf.v[1]);
    sf.v[2] = round_vec4f(sf.v[2]);
    mf.v[0] = round_vec4f(mf.v[0]);
    mf.v[1] = round_vec4f(mf.v[1]);
    mf.v[2] = round_vec4f(mf.v[2]);
    vec4f s_edges[3] = {
        sf.v[1] - sf.v[0],
        sf.v[2] - sf.v[1],
        sf.v[0] - sf.v[2]
    };
    vec4f m_edges[3] = {
        mf.v[1] - mf.v[0],
        mf.v[2] - mf.v[1],
        mf.v[0] - mf.v[2]
    };
    float temp, m_collide, s_collide;
    for (int i = 0; i < 3; i++) {
        vec4f temp_m = plane_intersect(sf.v[0], stable_n, mf.v[i], mf.v[i] + velocity, &temp);
        if ( dot_product(stable_n, cross_product(s_edges[0], sf.v[0] - temp_m)) > 0 ) continue;
        if ( dot_product(stable_n, cross_product(s_edges[1], sf.v[1] - temp_m)) > 0 ) continue;
        if ( dot_product(stable_n, cross_product(s_edges[2], sf.v[2] - temp_m)) > 0 ) continue;

        if ( temp < t && (temp >= 0 && temp <= 1) ) {
            t = temp;
            drawLine(temp_m, temp_m + (velocity * 1000), worldMatrix);
            displayPoint(temp_m, worldMatrix, 0xff0000);
            m_collide++;
            printf("Vector M collision\n");
        }

        vec4f temp_s = plane_intersect(mf.v[0], moving_n, sf.v[i], sf.v[i] + (velocity * -1.f), &temp);
        if ( dot_product(moving_n, cross_product(m_edges[0], mf.v[0] - temp_s)) > 0 ) continue;
        if ( dot_product(moving_n, cross_product(m_edges[1], mf.v[1] - temp_s)) > 0 ) continue;
        if ( dot_product(moving_n, cross_product(m_edges[2], mf.v[2] - temp_s)) > 0 ) continue;

        if ( temp < t && (temp >= 0 && temp <= 1) ) {
            t = temp;
            drawLine(temp_s, temp_s + (velocity * -1000), worldMatrix);
            displayPoint(temp_s, worldMatrix, 0x00ff00);
            s_collide++;
            printf("Vector S collision\n");
        }
    }

    if (s_collide || m_collide) {
        return t;
    }
}
float sweptDoubleTri(face *sf, face *mf, vec4f velocity, vec4f *n) {
    float t = __INT_MAX__;
    vec4f stable_n = norm_vec(triangle_cp(*sf));
    vec4f moving_n = norm_vec(triangle_cp(*mf));

    // float cbb = sweptTribbox(*sf, *mf, velocity);
    // return cbb;

    // float cv = sweptTriangleVectors(*sf, *mf, velocity);
    // return cv;

    // printf("\x1b[H\x1b[J");
    // printf("Before rounding\n");
    // logFace(*sf, 1, 0, 0);
    // printf("\n");
    // logFace(*mf, 1, 0, 0); 

    face stable_f, moving_f;

    stable_f.v[0] = sf->v[0];
    stable_f.v[1] = sf->v[1];
    stable_f.v[2] = sf->v[2];
    moving_f.v[0] = mf->v[0];
    moving_f.v[1] = mf->v[1];
    moving_f.v[2] = mf->v[2];
    // stable_f.v[0] = round_vec4f(sf->v[0]);
    // stable_f.v[1] = round_vec4f(sf->v[1]);
    // stable_f.v[2] = round_vec4f(sf->v[2]);
    // moving_f.v[0] = round_vec4f(mf->v[0]);
    // moving_f.v[1] = round_vec4f(mf->v[1]);
    // moving_f.v[2] = round_vec4f(mf->v[2]);

    vec4f s_edges[3] = {
        sf->v[1] - sf->v[0],
        sf->v[2] - sf->v[1],
        sf->v[0] - sf->v[2]
    };
    vec4f m_edges[3] = {
        mf->v[1] - mf->v[0],
        mf->v[2] - mf->v[1],
        mf->v[0] - mf->v[2]
    };

    displayFace(&stable_f, worldMatrix);
    // displayFace(&moving_f, worldMatrix);
    printf("\x1b[H\x1b[J");

    int ind_out = 1, ind_in = 1;
    float test;

    for (int x = 0; x < 3; x++) {
        if (ind_out > 2)
            ind_out = 0;

        vec4f normal = norm_vec(cross_product(m_edges[x], moving_f.v[x] + velocity));

        for (int y = 0; y < 3; y++) {
            if (ind_in > 2)
                ind_in = 0;

            vec4f r = round_vec4f(plane_intersect(moving_f.v[x], normal, stable_f.v[y], stable_f.v[ind_in], &test));
            if (test >= 0 && test <= 1) {

                vec4f normal2 = norm_vec(cross_product(s_edges[y], stable_f.v[y] + velocity));
                vec4f r2 = plane_intersect(r, normal2, moving_f.v[x], moving_f.v[ind_out], &test);
                if (test >= 0 && test <= 1) {

                    r2 = plane_intersect(moving_f.v[x], moving_n, r, r + (velocity * -1.f), &test);
                    drawLine(r2, r, worldMatrix);
                    vec4f vt = round_vec4f(r - r2) / velocity;

                    if (test < t && (test >= 0 && test <= 1)) {
                        float dot = fabsf(dot_product(moving_n, stable_n));
                        if (dot >= 0.0009) {
                            t = test;
                            drawLine(moving_f.v[x], moving_f.v[ind_out], worldMatrix);
                            printf("Edges  collision\n");
                            logFace(stable_f, 1, 0, 0);
                            printf("\n");
                            logFace(moving_f, 1, 0, 0);
                            // logVec4f(r);
                            // logVec4f(r2);
                            logVec4f(vt);
                        }
                    }
                }
            }
            ind_in++;
        }
        ind_out++;
    }
    return t;
}
const void terrainCollision(Mesh *terrain, Mesh *obj) {
    TerrainPointInfo tp = getTerrainPointData(terrain, (obj->cd.v[P] + obj->velocity), obj);

    obj->dm = getDimensionsLimits(obj->bbox.v, obj->bbox.v_indexes);

    vec4f min = { obj->cd.v[0][0], obj->dm.min[1], obj->cd.v[0][2], 1.f };
    vec4f t_near = (tp.pos - min) / obj->velocity;
    vec4f max = obj->velocity + min;

    if (obj->id == 1) {

        vec4f diff = min - tp.pos;

        // float move_dir = dot_product(tp.normal, norm_vec(obj->velocity));

        if (t_near[1] > 0 && t_near[1] <= 1) {
            obj->velocity *= t_near[1];

            Mat4x4 tm = translationMatrix(obj->velocity[0], obj->velocity[1], obj->velocity[2]);

            setvecsarrayxm(obj->cd.v, 4, tm);
            setvecsarrayxm(obj->bbox.v, obj->bbox.v_indexes, tm);
            setfacesarrayxm(obj->bbox.f, obj->bbox.f_indexes, tm);

            obj->grounded = 1;
            obj->falling_time = 0;

            float dot =  dot_product(tp.normal, obj->velocity);
            obj->velocity = (obj->velocity - (dot * tp.normal)) * terrain->fr_coef;
            return;
        }

        // drawLine(min, max, viewMatrix);
        // drawLine(min, min + (tp.normal * 100.f), viewMatrix);
        drawLine(obj->cd.v[0], obj->cd.v[0] + (tp.normal * 100.f), viewMatrix);
        drawLine(obj->cd.v[0], obj->cd.v[0] + obj->velocity, viewMatrix);
        displayBbox(obj, viewMatrix);

        if (obj->grounded) {
            obj->falling_time = 0;

            // if (diff[1] <= 0) {
                obj->velocity = (tp.pos - min);

                // float dot =  dot_product(tp.normal, obj->velocity);
                // obj->velocity = (obj->velocity - (dot * tp.normal)) * terrain->fr_coef;
            // } else if (diff[1] < 0) {
            //     obj->grounded = 0;
            // }
        }

        float dot = dot_product(min, tp.normal);
        if (dot < 0)
            printf("Below Surface\n");
        else
            printf("Above Surface\n");

        // float dot =  dot_product(tp.normal, obj->velocity);
        // obj->velocity = (obj->velocity - (dot * tp.normal)) * terrain->fr_coef;

        // printf("Grounded %d\n", obj->grounded);
        // logMesh(*obj);

        // logVec4f(t_near);
        // logVec4f(obj->dm.max);
        // logVec4f(obj->bbox.v[0]);
        // logVec4f(diff);
        return;
    } 



    float height_diff = tp.pos[1] - min[1];
    if ( height_diff >= 0 ) {
        obj->grounded = 1;
        obj->falling_time = 0;
    }
    if ( obj->grounded ) {
        Mat4x4 tm = translationMatrix(0, height_diff, 0);

        setvecsarrayxm(obj->cd.v, 4, tm);
        setvecsarrayxm(obj->bbox.v, obj->bbox.v_indexes, tm);
        setfacesarrayxm(obj->bbox.f, obj->bbox.f_indexes, tm);

        // float dot =  dot_product(tp.normal, obj->velocity);
        // obj->velocity = (obj->velocity - (dot * tp.normal)) * terrain->fr_coef;
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
                plane_n = norm_vec(triangle_cp(m->bbox.f[2]));

                if (dot_product(m->velocity, plane_n) > 0) {

                    // for (int y = 0; y < scene.m[2].bbox.f_indexes; y++) {
                        plane_n = norm_vec(triangle_cp(scene.m[2].bbox.f[1]));
                        float d0 = dist(scene.m[2].bbox.f[1].v[0], plane_n, m->cd.v[0]);
                        if (d0 >= 0) {
                            if (dot_product(m->velocity, plane_n) < 0) {

                                float temp = sweptDoubleTri(&scene.m[2].bbox.f[1], &m->bbox.f[2], m->velocity, &n);

                                if (temp < t) {
                                    t = temp;
                                    n = norm_vec(m->velocity * -1);
                                }
                            }
                        }
                    // }
                }
            // }

            // drawLine(m->cd.v[0], m->cd.v[0] + (n * 1000), worldMatrix);
            if (t < __INT_MAX__)
                printf("t: %f\n", t);
            if (t == 0.f) {
                // printf("Sliding...: %f\n", t);
                float dot =  dot_product(n, m->velocity);
                m->velocity = m->velocity - (dot * n);
                if (n[1] == 1)
                    m->falling_time = 0;
                return 1;
            } else if (t > 0.f && t <= 1.f) {
                // printf("Collision: %f\n", t);
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


