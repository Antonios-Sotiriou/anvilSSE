/* EDGE FUNCTION. */
float e1 = (P.x - V0.x) * (V1.y - V0.y) - (P.y - V0.y) * (V1.x - V0.x);
float e2 = (P.x - V1.x) * (V2.y - V1.y) - (P.y - V1.y) * (V2.x - V1.x);
float e3 = (P.x - V2.x) * (V0.y - V2.y) - (P.y - V2.y) * (V0.x - V2.x);

        if (d1 < 0 && d0 >= 0) {

            vec4f r = plane_intersect(plane_p, plane_n, line_start, line_end, &t);
            float e1 = dot_product(plane_n, cross_product(pf.v[1] - pf.v[0], pf.v[1] - r));
            float e2 = dot_product(plane_n, cross_product(pf.v[2] - pf.v[1], pf.v[2] - r));
            float e3 = dot_product(plane_n, cross_product(pf.v[0] - pf.v[2], pf.v[0] - r));
            printf("e1: %f    e2: %f    e3: %f\n", e1, e2, e3);
            if (e1 < 0 && e2 < 0 && e3 < 0) {
                displayPoint(r, worldMatrix, 0xff00a7);
                // displayFilledFace(&pf, worldMatrix);
                // printf("FRONT e1: %f    e2: %f    e3: %f\n", e1, e2, e3);
                return t; /* A new face is created. */
            }
        }


/* Without SAT */
float clippface(vec4f plane_p, vec4f plane_n, face in_f, face pf, vec4f velocity) {
    float d0, d1, t = 1000, test;

    Mat4x4 trans = translationMatrix(velocity[0], velocity[1], velocity[2]);
    face f = facexm(in_f, trans);

    vec4f line_start[3] = { f.v[0], f.v[1], f.v[2] };
    vec4f line_end[3] = { f.v[1], f.v[2], f.v[0] };

    vec4f moving_n = norm_vec(triangle_cp(in_f));
    float dot1 = dot_product(plane_n, moving_n);


    vec4f edges[3] = {
        pf.v[0] - pf.v[1],
        pf.v[1] - pf.v[2],
        pf.v[2] - pf.v[0]
    };
    if (dot1 != 0) {
        // displayFace(&in_f, worldMatrix);
        // displayFace(&pf, worldMatrix);
        // drawLine(pf.v[1], pf.v[2], worldMatrix);
        // drawLine(in_f.v[0], in_f.v[1], worldMatrix);
    
        int vec_col = 0;
        for (int i = 0; i < 3; i++) {
            d0 = dist(plane_p, plane_n, in_f.v[i]);
            d1 = dist(plane_p, plane_n, in_f.v[i] + velocity);
            float temp;
            if (d1 <= 0 && d0 >= 0) {
                vec4f temp_p = plane_intersect(plane_p, plane_n, in_f.v[i], in_f.v[i] + velocity, &temp);
                e1 = dot_product(plane_n, cross_product(edges[0], pf.v[0] - temp_p));
                e2 = dot_product(plane_n, cross_product(edges[1], pf.v[1] - temp_p));
                e3 = dot_product(plane_n, cross_product(edges[2], pf.v[2] - temp_p));

                if (e1 > 0 && e2 > 0 && e3 > 0) {

                    if (temp < t) {
                        t = temp;
                        // displayPoint(r, worldMatrix, 0x00ff28);
                        // displayPoint(temp_p, worldMatrix, 0xff00a7);
                        r = temp_p;
                        n = plane_n;
                        drawLine(temp_p, temp_p + -velocity * 100.f, worldMatrix);
                        displayFace(&in_f, worldMatrix);
                        displayFace(&pf, worldMatrix);
                        vec_col++;
                    }
                }
            }
        }
        if (vec_col)
            return t;
    }
    for (int x = 0; x < 3; x++) {
        d0 = dist(plane_p, plane_n, line_end[x]);
        d1 = dist(plane_p, plane_n, line_start[x]);
        if (d1 != d0 && (d1 <= 0 && d0 >= 0)) {
            r = plane_intersect(plane_p, plane_n, line_end[x], line_start[x], &test);
            e1 = dot_product(plane_n, cross_product(edges[0], pf.v[0] - r));
            e2 = dot_product(plane_n, cross_product(edges[1], pf.v[1] - r));
            e3 = dot_product(plane_n, cross_product(edges[2], pf.v[2] - r));

            float temp;
            if (e1 >= 0 && e2 >= 0 && e3 >= 0) {
                
                vec4f normal, temp_v;
                for (int i = 0; i < 3; i++) {
                    normal = norm_vec(cross_product(edges[i], pf.v[i] * (plane_n * 10.f)));


                    drawLine(r, r + (-normal * 100), worldMatrix);

                    temp_v = plane_intersect(pf.v[i], normal, r, r + (-normal * 10000.f), &test);
                    vec4f test1 = plane_intersect(in_f.v[i], moving_n, temp_v, temp_v + (-velocity), &temp);

                    if (temp < t) {
                        t = temp;
                        n = moving_n;
                        drawLine(test1, temp_v + (moving_n * 1000.f), worldMatrix);
                        displayFace(&in_f, worldMatrix);
                        displayFace(&pf, worldMatrix);

                    }
                }
            }
        }
    }
    return t;
}



/* With SAT */
float clippface(face sf, face mf, vec4f velocity, vec4f spivot, vec4f mpivot) {
    float d0, d1, t = __INT_MAX__;
    vec4f stable_n = norm_vec(triangle_cp(sf));
    vec4f moving_n = norm_vec(triangle_cp(mf));
    float dot1 = dot_product(stable_n, moving_n);

    displayFace(&mf, worldMatrix);
    displayFace(&sf, worldMatrix);

    // ############################################## VECTORS COLLIDE ##########################################################
    vec4f s_edges[3] = {
        sf.v[0] - sf.v[1],
        sf.v[1] - sf.v[2],
        sf.v[2] - sf.v[0]
    };
    vec4f m_edges[3] = {
        mf.v[0] - mf.v[1],
        mf.v[1] - mf.v[2],
        mf.v[2] - mf.v[0]
    };
    int m_collide = 0;
    for (int i = 0; i < 3; i++) {
        d0 = dist(sf.v[0], stable_n, mf.v[i]);
        d1 = dist(sf.v[0], stable_n, mf.v[i] + velocity);
        float temp;
        if (d1 <= 0 && d0 >= 0) {
            // drawLine(mf.v[i], mf.v[i] + (velocity * 1000), worldMatrix);

            vec4f temp_p = plane_intersect(sf.v[0], stable_n, mf.v[i], mf.v[i] + velocity, &temp);
            if ( dot_product(stable_n, cross_product(s_edges[0], sf.v[0] - temp_p)) <= 0 ) continue;
            if ( dot_product(stable_n, cross_product(s_edges[1], sf.v[1] - temp_p)) <= 0 ) continue;
            if ( dot_product(stable_n, cross_product(s_edges[2], sf.v[2] - temp_p)) <= 0 ) continue;

            if ( temp < t && (temp >= 0 && temp <= 1) ) {
                t = temp;
                r = temp_p;
                n = stable_n;
                drawLine(temp_p, temp_p + (velocity * 1000), worldMatrix);
                displayPoint(temp_p, worldMatrix, 0xff0000);
                m_collide++;
            }
        }
    }
    int s_collide = 0;
    if (!m_collide) {
        for (int i = 0; i < 3; i++) {
            d0 = dist(mf.v[0], moving_n, sf.v[i]);
            d1 = dist(mf.v[0], moving_n, sf.v[i] - velocity);
            float temp;
            if (d1 <= 0 && d0 >= 0) {
                // drawLine(sf.v[i], sf.v[i] - (velocity * 1000), worldMatrix);

                vec4f temp_p = plane_intersect(mf.v[0], moving_n, sf.v[i], sf.v[i] - velocity, &temp);
                if ( dot_product(moving_n, cross_product(m_edges[0], mf.v[0] - temp_p)) <= 0 ) continue;
                if ( dot_product(moving_n, cross_product(m_edges[1], mf.v[1] - temp_p)) <= 0 ) continue;
                if ( dot_product(moving_n, cross_product(m_edges[2], mf.v[2] - temp_p)) <= 0 ) continue;

                if ( temp < t && (temp >= 0 && temp <= 1) ) {
                    t = temp;
                    r = temp_p;
                    n = moving_n;
                    drawLine(temp_p, temp_p - (velocity * 1000), worldMatrix);
                    displayPoint(temp_p, worldMatrix, 0x00ff00);
                    s_collide++;
                }
            }
        }
    }
    if (m_collide || s_collide) {
        printf("Vector collision\n");
        return t;
    }
    // ############################################## VECTORS COLLIDE ##########################################################
    float dot, depth = __INT_MAX__;
    float min_m, max_m, min_s, max_s;
    vec4f norms[11] = {
        stable_n,
        moving_n,
        cross_product(m_edges[0], norm_vec(s_edges[0])),
        cross_product(m_edges[0], norm_vec(s_edges[1])),
        cross_product(m_edges[0], norm_vec(s_edges[2])),
        cross_product(m_edges[1], norm_vec(s_edges[0])),
        cross_product(m_edges[1], norm_vec(s_edges[1])),
        cross_product(m_edges[1], norm_vec(s_edges[2])),
        cross_product(m_edges[2], norm_vec(s_edges[0])),
        cross_product(m_edges[2], norm_vec(s_edges[1])),
        cross_product(m_edges[2], norm_vec(s_edges[2])),
    };
    vec4f temp_min_s, temp_min_m, temp_max_s, temp_max_m, temp_vm, temp_vs, temp_n;

    for (int x = 0; x < 11; x++) {
        min_m = __INT_MAX__, max_m = -__INT_MAX__, min_s = __INT_MAX__, max_s = -__INT_MAX__;
        if ((norms[x][0] + norms[x][1] + norms[x][2]) == 0)
            continue;
        for (int i = 0; i < 3; i++) {
            if ( (dot = dot_product(norms[x], mf.v[i])) < min_m ) {
                min_m = dot;
                temp_min_m = mf.v[i];
            }
            if (dot > max_m) {
                max_m = dot;
                temp_max_m = mf.v[i];
            }

            if ( (dot = dot_product(norms[x], sf.v[i])) < min_s ) {
                min_s = dot;
                temp_min_s = sf.v[i];
            }
            if (dot > max_s) {
                max_s = dot;
                temp_max_s = sf.v[i];
            }
        }

        if ( (min_m >= max_s) || (max_m <= min_s) ) {
            return -1;
        }

        float da = max_s - min_m;
        float db = max_m - min_s;
        float axis_depth = da < db ? da : db;
        if (axis_depth < depth) {
            depth = axis_depth;

            if (min_m >= max_s || (min_m < max_s && min_m > min_s)) {
                temp_vm = temp_min_m;
                temp_vs = temp_max_s;
                temp_n = norms[x];
                // printf("min_m: %f    min_s: %f ---- max_m: %f    max_s: %f\n", min_m, min_s, max_m, max_s);
            } else if (max_m <= min_s || (max_m > min_s && max_m < max_s)) {
                temp_vm = temp_max_m;
                temp_vs = temp_min_s;
                temp_n = -norms[x];
            }
        }
    }

    // if (dot_product(temp_n, velocity) <= 0)
    //     return -1;

    printf("Edge collision\n");
    return 1000;
}


                // if (isinf(test)) {
                //     printf("inf\n");
                //     drawLine(line_start[y], line_end[y], worldMatrix);
                //     drawLine(moving_f.v[x], moving_f.v[ind], worldMatrix);
                //     continue;
                // }
                // if (isnan(test)) {
                //     printf("nan\n");
                //     drawLine(line_start[y], line_end[y], worldMatrix);
                //     drawLine(moving_f.v[x], moving_f.v[ind], worldMatrix);
                //     continue;
                // }

        vec4f normal = norm_vec(cross_product(m_edges[x], moving_f.v[x] * velocity));
        vec4f plane_p = plane_intersect(stable_f.v[x], stable_n, moving_f.v[x] + velocity, moving_f.v[ind_out] + velocity, &test);
        if (test >= 0 && test <= 1) {
            displayPoint(plane_p, worldMatrix, 0xff0000);
            float dist1 = dist(stable_f.v[x], stable_n, moving_f.v[x] + velocity);
            float dist2 = dist(stable_f.v[x], stable_n, moving_f.v[ind_out] + velocity);
            vec4f nearest_v;
            if (dist1 > dist2) {
                plane_p = plane_intersect(stable_f.v[x], stable_n, moving_f.v[ind_out], moving_f.v[ind_out] + velocity, &test);
                nearest_v = moving_f.v[ind_out];
            } else {
                plane_p = plane_intersect(stable_f.v[x], stable_n, moving_f.v[x], moving_f.v[x] + velocity, &test);
                nearest_v = moving_f.v[x];
            }




/* LATEST APPROACH WHICH SEEMS PROMISING */
    displayFace(&stable_f, worldMatrix);
    // displayFace(&moving_f, worldMatrix);

    int ind_out = 1, ind_in = 1;
    float temp, test;

    for (int x = 0; x < 3; x++) {
        if (ind_out > 2)
            ind_out = 0;

        vec4f normal = norm_vec(cross_product(m_edges[x], moving_f.v[x] + velocity));

        for (int y = 0; y < 3; y++) {
            if (ind_in > 2)
                ind_in = 0;

            vec4f r = plane_intersect(moving_f.v[x], normal, stable_f.v[y], stable_f.v[ind_in], &test);
            if (test >= 0 && test <= 1) {

                vec4f r2 = plane_intersect(moving_f.v[x], moving_n, r, r + (velocity * -1.f), &test);
                if (test < t && (test >= 0 && test <= 1)) {
                    drawLine(r2, r, worldMatrix);

                    float dot = fabsf(dot_product(moving_n, stable_n));
                    if ( dot >= 0.0009) {
                        t = test;
                        drawLine(moving_f.v[x], moving_f.v[ind_out], worldMatrix);
                    }
                }
            }
            ind_in++;
        }
        ind_out++;
    }


/* LATEST APPROACH WHICH SEEMS PROMISING 2 */
    int ind_out = 1, ind_in = 1;
    float temp, test;

    for (int x = 0; x < 3; x++) {
        if (ind_out > 2)
            ind_out = 0;

        vec4f normal = norm_vec(cross_product(m_edges[x], moving_f.v[x] + velocity));

        for (int y = 0; y < 3; y++) {
            if (ind_in > 2)
                ind_in = 0;

            vec4f r = plane_intersect(moving_f.v[x], normal, stable_f.v[y], stable_f.v[ind_in], &test);
            if (test >= 0 && test <= 1) {

                vec4f normal2 = norm_vec(cross_product(s_edges[y], stable_f.v[y] + velocity));
                vec4f r2 = plane_intersect(r, normal2, moving_f.v[x], moving_f.v[ind_out], &test);
                if (test >= 0 && test <= 1) {

                    r2 = plane_intersect(r2, moving_n, r, r + (velocity * -1.f), &test);
                    drawLine(r2, r, worldMatrix);
                    if (test < t && (test >= 0 && test <= 1)) {

                        float dot = fabsf(dot_product(moving_n, stable_n));
                        if ( dot >= 0.0009) {
                            t = test;
                            drawLine(moving_f.v[x], moving_f.v[ind_out], worldMatrix);
                            logVec4f(r);
                            logVec4f(r2);
                        }
                    }
                }
            }
            ind_in++;
        }
        ind_out++;
    }


