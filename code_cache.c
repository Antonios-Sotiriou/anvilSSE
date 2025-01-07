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

const float clippface(vec4f plane_p, vec4f plane_n, face in_f, face pf, vec4f velocity) {
    float d0, d1, t;
    vec4f line_start, line_end;

    printf("in_f ------------\n");
    logFace(pf, 1, 0, 0);

    for (int i = 0; i < 3; i++) {
        line_start = in_f.v[i];
        line_end = in_f.v[i] + velocity * 100.f;
        d0 = dist(plane_p, plane_n, line_start);
        d1 = dist(plane_p, plane_n, line_end);

        drawLine(line_start, line_end, worldMatrix);

        if (d1 < 0 && d0 >= 0) {

            vec4f r = plane_intersect(plane_p, plane_n, line_start, line_end, &t);
            if (t >= 0 && t <= 1) {
                int e1 = dot_product(plane_n, cross_product(pf.v[0] - pf.v[1], pf.v[0] - r));
                int e2 = dot_product(plane_n, cross_product(pf.v[1] - pf.v[2], pf.v[1] - r));
                int e3 = dot_product(plane_n, cross_product(pf.v[2] - pf.v[0], pf.v[2] - r));

                if ( (e1 | e2 | e3) > 0) {
                    displayPoint(r, worldMatrix, 0xff00a7);
                    return t;
                }
            }
        }
    }
    return -1;
}

const float clippface(vec4f plane_p, vec4f plane_n, face in_f, face pf, vec4f velocity) {
    float d0, d1, t;
    vec4f line_start, line_end;

    vec4f v[3] = {
        { (int)(pf.v[0][0] + 0.5f), (int)(pf.v[0][1] + 0.5f), (int)(pf.v[0][2] + 0.5f), (int)(pf.v[0][3] + 0.5f) },
        { (int)(pf.v[1][0] + 0.5f), (int)(pf.v[1][1] + 0.5f), (int)(pf.v[1][2] + 0.5f), (int)(pf.v[1][3] + 0.5f) },
        { (int)(pf.v[2][0] + 0.5f), (int)(pf.v[2][1] + 0.5f), (int)(pf.v[2][2] + 0.5f), (int)(pf.v[2][3] + 0.5f) },
    };

    for (int i = 0; i < 3; i++) {
        line_start = in_f.v[i];
        line_end = in_f.v[i] + velocity * 100.f;
        d0 = dist(plane_p, plane_n, line_start);
        d1 = dist(plane_p, plane_n, line_end);

        drawLine(line_start, line_end, worldMatrix);

        if (d1 < 0 && d0 >= 0) {

            vec4i r = __builtin_convertvector(plane_intersect(plane_p, plane_n, line_start, line_end, &t) + 0.5f, vec4i);
            int e1 = dot_product(plane_n, cross_product(v[0] - v[1], v[0] - __builtin_convertvector(r, vec4f)));
            int e2 = dot_product(plane_n, cross_product(v[1] - v[2], v[1] - __builtin_convertvector(r, vec4f)));
            int e3 = dot_product(plane_n, cross_product(v[2] - v[0], v[2] - __builtin_convertvector(r, vec4f)));

            if ( (e1 | e2 | e3) > 0) {
                displayPoint(__builtin_convertvector(r, vec4f), worldMatrix, 0xff00a7);
                return t;
            }
        }
    }
    return -1.f;
}

const float clippface(vec4f plane_p, vec4f plane_n, face in_f, face pf, vec4f velocity, Mesh *m) {
    float d0, d1, t;
    vec4f line_start, line_end;

    printf("in_f ------------\n");
    logFace(in_f, 1, 0, 0);

    for (int i = 0; i < 3; i++) {
        line_start = in_f.v[i];
        line_end = in_f.v[i] + velocity;
        d0 = dist(plane_p, plane_n, line_start);
        d1 = dist(plane_p, plane_n, line_end);

        drawLine(line_start, line_end, worldMatrix);

        if (d1 < 0 && d0 >= 0) {

            vec4f r = plane_intersect(plane_p, plane_n, line_start, line_end, &t);
            float e1 = dot_product(plane_n, cross_product(pf.v[1] - pf.v[0], pf.v[1] - r));
            float e2 = dot_product(plane_n, cross_product(pf.v[2] - pf.v[1], pf.v[2] - r));
            float e3 = dot_product(plane_n, cross_product(pf.v[0] - pf.v[2], pf.v[0] - r));

            if (e1 < 0 && e2 < 0 && e3 < 0) {
                displayPoint(r, worldMatrix, 0xff00a7);
                return t;
            }
        }
    }
    plane_n = norm_vec(triangle_cp(in_f));
    plane_p = m->cd.v[0] + (plane_n * m->scale);
    for (int i = 0; i < 3; i++) {
        line_start = pf.v[i];
        line_end = pf.v[i] + (-velocity * 100.f);
        d0 = dist(plane_p, plane_n, line_start);
        d1 = dist(plane_p, plane_n, line_end);

        drawLine(line_start, line_end, worldMatrix);

        if (d1 < 0 && d0 >= 0) {

            vec4f r = plane_intersect(plane_p, plane_n, line_start, line_end, &t);
            float e1 = dot_product(plane_n, cross_product(in_f.v[1] - in_f.v[0], in_f.v[1] - r));
            float e2 = dot_product(plane_n, cross_product(in_f.v[2] - in_f.v[1], in_f.v[2] - r));
            float e3 = dot_product(plane_n, cross_product(in_f.v[0] - in_f.v[2], in_f.v[0] - r));

            if (e1 < 0 && e2 < 0 && e3 < 0) {
                displayPoint(r, worldMatrix, 0xff00a7);
                return t;
            }
        }
    }
    return -1.f;
}

        
/* Check if starting and ending possition of  a vector with velocity collides with the triangle */
const float clippface(vec4f plane_p, vec4f plane_n, face in_f, face pf, vec4f velocity) {
    float d0, d1, t;
    vec4f line_start, line_end;

    for (int i = 0; i < 3; i++) {
        line_start = in_f.v[i];
        line_end = in_f.v[i] + velocity * 100.f;
        d0 = dist(plane_p, plane_n, line_start);
        d1 = dist(plane_p, plane_n, line_end);

        drawLine(line_start, line_end, worldMatrix);
        face f = pf;

        if (d1 < 0 && d0 >= 0) {

            vec4f r = plane_intersect(plane_p, plane_n, line_start, line_end, &t);
            float e1 = (r[0] - f.v[0][0]) * (f.v[1][1] - f.v[0][1]) - (r[1] - f.v[0][1]) * (f.v[1][0] - f.v[0][0]);
            float e2 = (r[0] - f.v[1][0]) * (f.v[2][1] - f.v[1][1]) - (r[1] - f.v[1][1]) * (f.v[2][0] - f.v[1][0]);
            float e3 = (r[0] - f.v[2][0]) * (f.v[0][1] - f.v[2][1]) - (r[1] - f.v[2][1]) * (f.v[0][0] - f.v[2][0]);

            printf("e1: %f    e2: %f    e3: %f\n", e1, e2, e3);
            if (e1 >= 0 && e2 >= 0 && e3 >= 0) {
                displayPoint(r, worldMatrix, 0xff00a7);
                displayFilledFace(&pf, worldMatrix);
                // printf("FRONT e1: %f    e2: %f    e3: %f\n", e1, e2, e3);
                return t; /* A new face is created. */
            } else if (e1 <= 0 && e2 <= 0 && e3 <= 0) {
                displayPoint(r, worldMatrix, 0xff00a7);
                displayFilledFace(&pf, worldMatrix);
                // printf("BACK  e1: %f    e2: %f    e3: %f\n", e1, e2, e3);
                return t; /* Two new faces are created. */
            }
        }
    }
    return -1.f;
}

const float clippface(vec4f plane_p, vec4f plane_n, face in_f, face pf) {
    int inside_points[3];     int inside_count = 0;
    int outside_points[3];    int outside_count = 0;

    // Get signed distance of each point in triangle to plane.
    float d0 = dist(plane_p, plane_n, in_f.v[0]);
    float d1 = dist(plane_p, plane_n, in_f.v[1]);
    float d2 = dist(plane_p, plane_n, in_f.v[2]);

    if (d0 >= 0) {
        inside_points[inside_count] = 0;
        inside_count++;
    } else {
        outside_points[outside_count] = 0;
        outside_count++;
    }
    if (d1 >= 0) {
        inside_points[inside_count] = 1;
        inside_count++;
    } else {
        outside_points[outside_count] = 1;
        outside_count++;
    }
    if (d2 >= 0) {
        inside_points[inside_count] = 2;
        inside_count++;
    } else {
        outside_points[outside_count] = 2;
        outside_count++;
    }

    float t;
    if (inside_count == 0) {
        return -1.f; /* face is outside and must be ignored. */
    } else if (inside_count == 3) {
        return 3.f; /* face is inside and it needs no clipping. */
    } else if (inside_count == 1 && outside_count == 2) {

        // if ( inside_points[0] != 1 )
        //     swap(&outside_points[0], &outside_points[1], 4);

        vec4f r = plane_intersect(plane_p, plane_n, in_f.v[inside_points[0]], in_f.v[outside_points[1]], &t);
        // vec4f r2 = plane_intersect(plane_p, plane_n, in_f.v[inside_points[0]], in_f.v[outside_points[0]], &t);
        
        // displayPoint(r, worldMatrix, 0xff00a7);
        float e1 = (r[0] - pf.v[0][0]) * (pf.v[1][1] - pf.v[0][1]) - (r[1] - pf.v[0][1]) * (pf.v[1][0] - pf.v[0][0]);
        float e2 = (r[0] - pf.v[1][0]) * (pf.v[2][1] - pf.v[1][1]) - (r[1] - pf.v[1][1]) * (pf.v[2][0] - pf.v[1][0]);
        float e3 = (r[0] - pf.v[2][0]) * (pf.v[0][1] - pf.v[2][1]) - (r[1] - pf.v[2][1]) * (pf.v[0][0] - pf.v[2][0]);
        
        if (e1 >= 0 && e2 >= 0 && e3 >= 0) {
            displayPoint(r, worldMatrix, 0xff00a7);
            return t; /* A new face is created. */
        } else if (e1 <= 0 && e2 <= 0 && e3 <= 0) {
            displayPoint(r, worldMatrix, 0xff00a7);
            return t; /* Two new faces are created. */
        }
    } else if (inside_count == 2 && outside_count == 1) {
        // if ( outside_points[0] != 1 )
        //     swap(&inside_points[0], &inside_points[1], 4);

        vec4f r = plane_intersect(plane_p, plane_n, in_f.v[inside_points[0]], in_f.v[outside_points[0]], &t);
        
        // displayPoint(r, worldMatrix, 0xff00a7);
        float e1 = (r[0] - pf.v[0][0]) * (pf.v[1][1] - pf.v[0][1]) - (r[1] - pf.v[0][1]) * (pf.v[1][0] - pf.v[0][0]);
        float e2 = (r[0] - pf.v[1][0]) * (pf.v[2][1] - pf.v[1][1]) - (r[1] - pf.v[1][1]) * (pf.v[2][0] - pf.v[1][0]);
        float e3 = (r[0] - pf.v[2][0]) * (pf.v[0][1] - pf.v[2][1]) - (r[1] - pf.v[2][1]) * (pf.v[0][0] - pf.v[2][0]);
        
        if (e1 >= 0 && e2 >= 0 && e3 >= 0) {
            displayPoint(r, worldMatrix, 0xff00a7);
            return t; /* Two new faces are created. */
        } else if (e1 <= 0 && e2 <= 0 && e3 <= 0) {
            displayPoint(r, worldMatrix, 0xff00a7);
            return t; /* Two new faces are created. */
        }
    }
    return -1.f;
}
    
    /* Create a depth and stencil texture.*/
    glGenFramebuffers(1, &clickSelectMapFBO);

    
    glGenTextures(1, &clickSelectColorMap);
    printf("clickSelectColorMap: %d\n", clickSelectColorMap);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, clickSelectColorMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Create a depth texture to use is for clickSelect.*/
    glGenTextures(1, &clickSelectDepthMap);
    printf("clickSelectDepthMap: %d\n", clickSelectDepthMap);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, clickSelectDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Attach the generated 2D Texture to our Shadow Map framebuffer's depth buffer */
    glBindFramebuffer(GL_FRAMEBUFFER, clickSelectMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, clickSelectColorMap, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, clickSelectDepthMap, 0);
    if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
        perror("Incomplete clickSelectMapFBO ");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);





    /* Create a 2D Texture to use it for clickSelect aka. (pick objects on mouse click) SEPERATE DEPTH AND STENCIL. */
    glGenTextures(1, &clickSelectColorMap);
    printf("clickSelectColorMap: %d\n", clickSelectColorMap);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, clickSelectColorMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Create a depth texture to use is for clickSelect.*/
    glGenTextures(1, &clickSelectDepthMap);
    printf("clickSelectDepthMap: %d\n", clickSelectDepthMap);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, clickSelectDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Create a stencil texture to experiment with it.*/
    glGenTextures(1, &clickSelectStencilMap);
    printf("clickSelectStencilMap: %d\n", clickSelectStencilMap);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, clickSelectStencilMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX8, WIDTH, HEIGHT, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Attach the generated 2D Texture to our Shadow Map framebuffer's depth buffer */
    glBindFramebuffer(GL_FRAMEBUFFER, clickSelectMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, clickSelectColorMap, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, clickSelectDepthMap, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, clickSelectStencilMap, 0);
    if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
        perror("Incomplete clickSelectMapFBO ");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



// To compile in Linux i installed build-essential libwayland-dev libxkbcommon-dev xorg-dev
// then we must run make command from inside the out directory.
const int obbCollision(Mesh *m) {
    const int pks_num = scene.t.quads[m->quadIndex].members_indexes;
    // printf("\x1b[H\x1b[J");

    for (int i = 0; i < pks_num; i++) {
        int pk = scene.t.quads[m->quadIndex].members[i];
        if (pk != m->id && pk != 6) {

            for (int j = 0; j < scene.m[pk].bbox.f_indexes; j++) {

                vec4f plane_n = norm_vec(triangle_cp(scene.m[pk].bbox.f[j]));
                vec4f plane_p = scene.m[pk].cd.v[0] + (plane_n * scene.m[pk].scale);

                for (int d = 0; d < m->bbox.f_indexes; d++) {

                    float d0 = dist(plane_p, plane_n, (m->cd.v[0] + m->velocity));
                    if (d0 >= 0 && dot_product(m->velocity, plane_n) < 0) {

                        vec4f anti_plane_n = norm_vec(triangle_cp(m->bbox.f[d]));
                        vec4f anti_plane_p = m->cd.v[0] + (anti_plane_n * m->scale);
                        // displayFilledFace(&scene.m[pk].bbox.f[j], worldMatrix);

                        if (dot_product(m->velocity, anti_plane_n) > 0) {
                            // displayFilledFace(&m->bbox.f[d], worldMatrix);

                            float t = clippface(plane_p, plane_n, m->bbox.f[d], scene.m[pk].bbox.f[j], m->velocity);
                            // const float t = clippface(anti_plane_p, anti_plane_n, scene.m[pk].bbox.f[j], m->bbox.f[d], m->velocity * -1);
                            // printf("t...: %f\n", t);
                            // if (t > 0 && t < 0.0001)
                            //     t = 0;
                            if (t == 0.f) {
                                printf("\x1b[H\x1b[J");
                                printf("Sliding...: %f\n", t);
                                logVec4f(plane_n);
                                printf("e1: %f    e2: %f    e3: %f\n", e1, e2, e3);
                                logVec4f(r);
                                printf("stable ------------\n");
                                logFace(scene.m[pk].bbox.f[j], 1, 0, 0);
                                printf("moving ------------\n");
                                logFace(m->bbox.f[d], 1, 0, 0);

                                float dot =  dot_product(plane_n, m->velocity);
                                m->velocity = m->velocity - (dot * plane_n);
                                return 0;
                            } else if (t > 0.f && t <= 1.f) {
                                printf("\x1b[H\x1b[J");
                                printf("Collision: %f\n", t);
                                logVec4f(plane_n);
                                printf("e1: %f    e2: %f    e3: %f\n", e1, e2, e3);
                                logVec4f(r);
                                printf("stable ------------\n");
                                logFace(scene.m[pk].bbox.f[j], 1, 0, 0);
                                printf("moving ------------\n");
                                logFace(m->bbox.f[d], 1, 0, 0);

                                m->velocity *= t;
                                Mat4x4 trans = translationMatrix(m->velocity[0], m->velocity[1], m->velocity[2]);
                                setvecsarrayxm(m->cd.v, 4, trans);
                                setvecsarrayxm(m->bbox.v, m->bbox.v_indexes, trans);
                                setfacesarrayxm(m->bbox.f, m->bbox.f_indexes, trans);

                                float dot =  dot_product(plane_n, m->velocity);
                                m->velocity = m->velocity - (dot * plane_n);
                                return 0;
                            } else {
                                printf("\x1b[H\x1b[J");
                                printf("Nothing: %f\n", t);
                                logVec4f(plane_n);
                                printf("e1: %f    e2: %f    e3: %f\n", e1, e2, e3);
                                logVec4f(r);
                                printf("stable ------------\n");
                                logFace(scene.m[pk].bbox.f[j], 1, 0, 0);
                                printf("moving ------------\n");
                                logFace(m->bbox.f[d], 1, 0, 0);         
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}








const float clippface(vec4f plane_p, vec4f plane_n, face in_f, face pf, vec4f velocity) {
    float d0, d1, t;

    Mat4x4 trans = translationMatrix(velocity[0], velocity[1], velocity[2]);
    face f = facexm(in_f, trans);

    vec4f line_start[3] = { f.v[0], f.v[1], f.v[2] };
    vec4f line_end[3] = { f.v[1], f.v[2], f.v[0] };
    for (int i = 0; i < 3; i++) {

        d0 = dist(plane_p, plane_n, line_start[i]);
        d1 = dist(plane_p, plane_n, line_end[i]);

        drawLine(line_start[i], line_end[i], worldMatrix);
        // printf("d0: %f    d1: %f\n", d0, d1);

        if (d1 <= 0.f && d0 >= 0.f) {
            // displayFilledFace(&pf, worldMatrix);
            r = plane_intersect(plane_p, plane_n, line_start[i], line_end[i], &t);
            e1 = dot_product(plane_n, cross_product(pf.v[0] - pf.v[1], pf.v[0] - r));
            e2 = dot_product(plane_n, cross_product(pf.v[1] - pf.v[2], pf.v[1] - r));
            e3 = dot_product(plane_n, cross_product(pf.v[2] - pf.v[0], pf.v[2] - r));
            // printf("e1: %f    e2: %f    e3: %f\n", e1, e2, e3);
            if (e1 > 0 && e2 > 0 && e3 > 0) {
                displayPoint(r, worldMatrix, 0xff00a7);
                // displayFilledFace(&pf, worldMatrix);
                // printf("e1: %f    e2: %f    e3: %f    t; %f\n", e1, e2, e3, t);
                return t;
            }
        }
    }
    return -1.f;
}


const int obbCollision(Mesh *m) {
    const int pks_num = scene.t.quads[m->quadIndex].members_indexes;
    // printf("\x1b[H\x1b[J");

    for (int i = 0; i < pks_num; i++) {
        int pk = scene.t.quads[m->quadIndex].members[i];
        if (pk != m->id && pk != 6) {

            // for (int j = 0; j < scene.m[pk].bbox.f_indexes; j++) {

                vec4f plane_n = norm_vec(triangle_cp(scene.m[pk].bbox.f[0]));
                vec4f plane_p = scene.m[pk].cd.v[0] + (plane_n * scene.m[pk].scale);

                // for (int d = 0; d < m->bbox.f_indexes; d++) {

                    float d0 = dist(plane_p, plane_n, (m->cd.v[0] + m->velocity));
                    if (d0 >= 0 && dot_product(m->velocity, plane_n) < 0) {

                        vec4f anti_plane_n = norm_vec(triangle_cp(m->bbox.f[2]));
                        vec4f anti_plane_p = m->cd.v[0] + (anti_plane_n * m->scale);
                        displayFilledFace(&scene.m[pk].bbox.f[0], worldMatrix);

                        if (dot_product(m->velocity, anti_plane_n) > 0) {
                            displayFilledFace(&m->bbox.f[2], worldMatrix);

                            float t = clippface(plane_p, plane_n, m->bbox.f[2], scene.m[pk].bbox.f[0], m->velocity);
                            // const float t = clippface(anti_plane_p, anti_plane_n, scene.m[pk].bbox.f[j], m->bbox.f[d], m->velocity * -1);
                            // printf("t...: %f\n", t);
                            // if (t > 0 && t < 0.0001)
                            //     t = 0;
                            printf("\x1b[H\x1b[J");
                            printf("e1: %f    e2: %f    e3: %f\n", e1, e2, e3);
                            logVec4f(plane_n);
                            logVec4f(r);
                            printf("stable ------------\n");
                            logFace(scene.m[pk].bbox.f[0], 1, 0, 0);
                            printf("moving ------------\n");
                            logFace(m->bbox.f[2], 1, 0, 0);
                            if (t == 0.f) {
                                printf("Sliding...: %f\n", t);

                                float dot =  dot_product(plane_n, m->velocity);
                                m->velocity = m->velocity - (dot * plane_n);
                                return 0;
                            } else if (t > 0.f && t <= 1.f) {
                                printf("Collision: %f\n", t);

                                m->velocity *= 0;
                                Mat4x4 trans = translationMatrix(m->velocity[0], m->velocity[1], m->velocity[2]);
                                setvecsarrayxm(m->cd.v, 4, trans);
                                setvecsarrayxm(m->bbox.v, m->bbox.v_indexes, trans);
                                setfacesarrayxm(m->bbox.f, m->bbox.f_indexes, trans);

                                float dot =  dot_product(plane_n, m->velocity);
                                m->velocity = m->velocity - (dot * plane_n);
                                return 0;
                            } else {
                                printf("Nothing: %f\n", t);    
                            }
                        }
                    }
                // }
            // }
        }
    }
    return 0;
}

