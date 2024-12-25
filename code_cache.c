/* EDGE FUNCTION. */
float e1 = (P.x - V0.x) * (V1.y - V0.y) - (P.y - V0.y) * (V1.x - V0.x);
float e2 = (P.x - V1.x) * (V2.y - V1.y) - (P.y - V1.y) * (V2.x - V1.x);
float e3 = (P.x - V2.x) * (V0.y - V2.y) - (P.y - V2.y) * (V0.x - V2.x);


/* Check if starting and ending possition of  a vector with velocity collides with the triangle */
const float clippface(vec4f plane_p, vec4f plane_n, face in_f, face pf, vec4f velocity) {
    float d0, d1, t;
    vec4f line_start, line_end;

    for (int i = 0; i < 3; i++) {
        line_start = in_f.v[i];
        line_end = in_f.v[i] + velocity;
        d0 = dist(plane_p, plane_n, line_start);
        d1 = dist(plane_p, plane_n, line_end);

        if (d1 < 0 && d0 >= 0) {
            printf("d0: %f    d1: %f\n", d0, d1);
            vec4f r = plane_intersect(plane_p, plane_n, line_start, line_end, &t);
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

