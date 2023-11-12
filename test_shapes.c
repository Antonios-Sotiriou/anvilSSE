#include "headers/test_shapes.h"

extern XWindowAttributes wa;
extern float NPlane, FPlane;
extern vec4f camera[4];
extern Mat4x4 reperspMat;
#include <math.h>
#include "headers/matrices.h"
#include "headers/logging.h"
const void createCube(Mesh *c) {
    size_t face_size = sizeof(face);
    c->v = malloc(16 * 8);
    c->n = malloc(16 * 8);
    c->t = malloc(8 * 14);
    c->f = malloc(face_size * 12);

    vec4f vectors[8] = {
        { -1.000000, -1.000000, -1.000000, 1.0 },
        { -1.000000, 1.000000, -1.000000, 1.0 },
        { -1.000000, -1.000000, 1.000000, 1.0 },
        { -1.000000, 1.000000, 1.000000, 1.0 },
        { 1.000000, -1.000000, -1.000000, 1.0 },
        { 1.000000, 1.000000, -1.000000, 1.0 },
        { 1.000000, -1.000000, 1.000000, 1.0 },
        { 1.000000, 1.000000, 1.000000, 1.0 }
    };
    vec4f normals[8] = {
        { -0.5774, -0.5774, 0.5774, 0.0 },
        { -0.5774, -0.5774, -0.5774, 0.0 },
        { 0.5774, -0.5774, -0.5774, 0.0 },
        { 0.5774, 0.5774, 0.5774, 0.0 },
        { -0.5774, 0.5774, 0.5774, 0.0 },
        { 0.5774, 0.5774, -0.5774, 0.0 },
        { 0.5774, -0.5774, 0.5774, 0.0 },
        { -0.5774, 0.5774, -0.5774, 0.0 }
    };
    vec2f textures[14] = {
        { 0.250869, 0.999111 },
        { 0.000272, 0.664772 },
        { 0.999470, 0.666021 },
        { 0.000071, 0.334027 },
        { 0.250490, 0.000967 },
        { 0.999545, 0.334644 },
        { 0.251146, 0.664645 },
        { 0.250069, 0.333989 },
        { 0.499504, 0.999136 },
        { 0.749957, 0.666010 },
        { 0.499506, 0.000737 },
        { 0.749957, 0.334637 },
        { 0.499504, 0.666010 },
        { 0.499493, 0.334624 }
    };
    face faces[12] = {
        { .a = { 2, 6, 0 }, .b = { 0, 0, 1 }, .c = { 4, 8, 2 } },
        { .a = { 7, 13, 3 }, .b = { 3, 7, 4 }, .c = { 2, 6, 0 } },
        { .a = { 5, 11, 5 }, .b = { 7, 13, 3 }, .c = { 6, 12, 6 } },
        { .a = { 7, 13, 3 }, .b = { 5, 10, 5 }, .c = { 1, 4, 7 } },
        { .a = { 3, 7, 4 }, .b = { 1, 3, 7 }, .c = { 0, 1, 1 } },
        { .a = { 1, 5, 7 }, .b = { 5, 11, 5 }, .c = { 4, 9, 2 } },
        { .a = { 2, 6, 0 }, .b = { 4, 8, 2 }, .c = { 6, 12, 6 } },
        { .a = { 7, 13, 3 }, .b = { 2, 6, 0 }, .c = { 6, 12, 6 } },
        { .a = { 5, 11, 5 }, .b = { 6, 12, 6 }, .c = { 4, 9, 2 } },
        { .a = { 7, 13, 3 }, .b = { 1, 4, 7 }, .c = { 3, 7, 4 } },
        { .a = { 3, 7, 4 }, .b = { 0, 1, 1 }, .c = { 2, 6, 0 } },
        { .a = { 1, 5, 7 }, .b = { 4, 9, 2 }, .c = { 0, 2, 1 } }
    };
    c->v_indexes = 8;
    c->n_indexes = 8;
    c->t_indexes = 14;
    c->f_indexes = 12;

    memcpy(c->v, vectors, 16 * 8);
    memcpy(c->n, normals, 16 * 8);
    memcpy(c->t, textures, 8 * 14);
    memcpy(c->f, faces, face_size * 12);
}
const void viewFrustum(Mesh *c) {
    size_t face_size = sizeof(face);
    c->v = malloc(16 * 8);
    c->n = malloc(16 * 8);
    c->t = malloc(8 * 14);
    c->f = malloc(face_size * 12);

    vec4f vectors[8] = {
        { 1.000000f, 1.000000f, -1.000000f, 1.f },
        { 1.000000f, -1.000000f, -1.000000f, 1.f },
        { 1.000000f, 1.000000f, 1.000000f, 1.f },
        { 1.000000f, -1.000000f, 1.000000f, 1.f },
        { -1.000000f, 1.000000f, -1.000000f, 1.f },
        { -1.000000f, -1.000000f, -1.000000f, 1.f },
        { -1.000000f, 1.000000f, 1.000000f, 1.f },
        { -1.000000f, -1.000000f, 1.000000f, 1.f }
    };
    float aspectRatio = (float)wa.width / (float)wa.height;
    float fovRadius = 1.f / tanf(45.f * 0.5f / 180.0f * 3.14159f);

    vec4f nearcenter = (camera[0] + camera[3]) * NPlane;
    vec4f farcenter = (camera[0] + camera[3]) * 100;

    float nearHeight = 2.f * tan(fovRadius / 2.f);
    float farHeight = 2.f * tan(fovRadius / 2.f) * 100;
    float nearWidth = nearHeight * aspectRatio;
    float farWidth = farHeight * aspectRatio;

    vectors[2] = nearcenter + (camera[2] * (nearHeight * 0.5f)) + (camera[1] * (nearWidth * 0.5f));
    vectors[3] = nearcenter - (camera[2] * (nearHeight * 0.5f)) + (camera[1] * (nearWidth * 0.5f));
    vectors[6] = nearcenter + (camera[2] * (nearHeight * 0.5f)) - (camera[1] * (nearWidth * 0.5f));
    vectors[7] = nearcenter - (camera[2] * (nearHeight * 0.5f)) - (camera[1] * (nearWidth * 0.5f));

    vectors[0] = farcenter + (camera[2] * (farHeight * 0.5f)) + (camera[1] * (farWidth * 0.5f));
    vectors[1] = farcenter - (camera[2] * (farHeight * 0.5f)) + (camera[1] * (farWidth * 0.5f));
    vectors[4] = farcenter + (camera[2] * (farHeight * 0.5f)) - (camera[1] * (farWidth * 0.5f));
    vectors[5] = farcenter - (camera[2] * (farHeight * 0.5f)) - (camera[1] * (farWidth * 0.5f));

    
    // Mat4x4 test = lookat(camera[0], camera[1], camera[2], camera[3]);
    // Mat4x4 ppm = perspectiveMatrix(fovRadius, aspectRatio, 0.1f, 1.f);
    // Mat4x4 tr = mxm(test, ppm);
    for (int i = 0; i < 8; i++) {
        // vectors[i] = vecxm(vectors[i], inverse_mat(tr));
        // vectors[i] /= vectors[i][3];
        logVec4f(vectors[i]);
    }
    vec4f normals[8] = {
        { -0.5774f, -0.5774f, -0.5774f, 0.f },
        { 0.5774f, -0.5774f, 0.5774f, 0.f },
        { -0.5774f, -0.5774f, 0.5774f, 0.f },
        { 0.5774f, 0.5774f, -0.5774f, 0.f },
        { -0.5774f, 0.5774f, -0.5774f, 0.f },
        { 0.5774f, 0.5774f, 0.5774f, 0.f },
        { 0.5774f, -0.5774f, -0.5774f, 0.f },
        { -0.5774f, 0.5774f, 0.5774f, 0.f }
    };
    vec2f textures[14] = {
        { 0.250869, 0.999111 },
        { 0.000272, 0.664772 },
        { 0.999470, 0.666021 },
        { 0.000071, 0.334027 },
        { 0.250490, 0.000967 },
        { 0.999545, 0.334644 },
        { 0.251146, 0.664645 },
        { 0.250069, 0.333989 },
        { 0.499504, 0.999136 },
        { 0.749957, 0.666010 },
        { 0.499506, 0.000737 },
        { 0.749957, 0.334637 },
        { 0.499504, 0.666010 },
        { 0.499493, 0.334624 }
    };
    face faces[12] = {
        { .a = { 2, 6, 0 }, .b = { 4, 8, 1 }, .c = { 0, 0, 2 } },
        { .a = { 7, 13, 3 }, .b = { 2, 6, 0 }, .c = { 3, 7, 4 } },
        { .a = { 5, 11, 5 }, .b = { 6, 12, 6 }, .c = { 7, 13, 3 } },
        { .a = { 7, 13, 3 }, .b = { 1, 4, 7 }, .c = { 5, 10, 5 } },
        { .a = { 3, 7, 4 }, .b = { 0, 1, 2 }, .c = { 1, 3, 7 } },
        { .a = { 1, 5, 7 }, .b = { 4, 9, 1 }, .c = { 5, 11, 5 } },
        { .a = { 2, 6, 0 }, .b = { 6, 12, 6 }, .c = { 4, 8, 1 } },
        { .a = { 7, 13, 3 }, .b = { 6, 12, 6 }, .c = { 2, 6, 0 } },
        { .a = { 5, 11, 5 }, .b = { 4, 9, 1 }, .c = { 6, 12, 6 } },
        { .a = { 7, 13, 3 }, .b = { 3, 7, 4 }, .c = { 1, 4, 7 } },
        { .a = { 3, 7, 4 }, .b = { 2, 6, 0 }, .c = { 0, 1, 2 } },
        { .a = { 1, 5, 7 }, .b = { 0, 2, 2 }, .c = { 4, 9, 1 } }
    };
    c->v_indexes = 8;
    c->n_indexes = 8;
    c->t_indexes = 14;
    c->f_indexes = 12;

    memcpy(c->v, vectors, 16 * 8);
    memcpy(c->n, normals, 16 * 8);
    memcpy(c->t, textures, 8 * 14);
    memcpy(c->f, faces, face_size * 12);
}


