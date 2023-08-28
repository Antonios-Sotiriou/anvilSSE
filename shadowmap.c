#include "headers/shadowmap.h"

#include "headers/logging.h"

extern XWindowAttributes wa;
extern int HALFW;
extern int HALFH;
extern float *shadow_buffer, shadow_bias;
extern Mat4x4 lookAt, lightMat, reperspMat;

const void createShadowmap(Mesh m) {
    for (int c = 0; c < m.f_indexes; c++) {
        /* Creating 2Arrays for X and Y values to sort them. */
        Srt srt[3] = {
            { .y =  m.f[c].v[0][1], .index = 0},
            { .y =  m.f[c].v[1][1], .index = 1},
            { .y =  m.f[c].v[2][1], .index = 2}
        };

        /* Sorting the values from smaller to larger y. */
        Srt temp;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                if (srt[i].y < srt[j].y) {
                    temp = srt[j];
                    srt[j] = srt[i];
                    srt[i] = temp;
                }

        shadowface(m.f[c], srt);
    }
}
const void shadowface(const face f, const Srt srt[]) {
    vec4i mask = { 1, 2, 0, 3 };
    vec4i txs = { f.v[srt[0].index][0], f.v[srt[1].index][0], f.v[srt[2].index][0], 0 };
    vec4i tys = { f.v[srt[0].index][1], f.v[srt[1].index][1], f.v[srt[2].index][1], 0 };
    vec4f tzs = { f.v[srt[0].index][2], f.v[srt[1].index][2], f.v[srt[2].index][2], 0 };
    vec4i txmx = __builtin_shuffle(txs, mask) - txs;
    vec4i tymy = __builtin_shuffle(tys, mask) - tys;
    vec4f tzmz = __builtin_shuffle(tzs, mask) - tzs;

    const int orient = (txmx[0] * tymy[2]) - (tymy[0] * txmx[2]);
    float ma = (float)txmx[0] / tymy[0];
    float mb = (float)txmx[2] / tymy[2];
    float za = tzmz[0] / tymy[0];
    float zb = tzmz[2] / tymy[2];
    if (orient < 0) {
        swap(&ma, &mb, sizeof(float));
        swap(&za, &zb, sizeof(float));
    }

    const int y_start = tys[0];
    const int y_end1 = tys[1];
    const int y_end2 = tys[2];

    int yA = 0;
    if (tymy[0] != 0)
        for (int y = y_start; y < y_end1; y++) {
            const int padySB = y * wa.width;

            int x_start = (ma * yA) + txs[0];
            int x_end = (mb * yA) + txs[0];

            const float z1 = (za * yA) + tzs[0];
            const float z2 = (zb * yA) + tzs[0];

            const float xexs = x_end - x_start;
            const float z2z1 = z2 - z1;
            float xxs = 0.0;
            for (int x = x_start; x <= x_end; x++) {

                const float barycentric = xxs / xexs;
                const float depthZ = z1 + (barycentric * z2z1);

                const int padxSB = padySB + x;
                if ( depthZ < shadow_buffer[padxSB] ) {

                    shadow_buffer[padxSB] = depthZ;
                }
                xxs += 1.0;
            }
            yA++;
        }

    if (tymy[1] == 0)
        return;

    ma = (float)txmx[1] / tymy[1];
    mb = (float)txmx[2] / tymy[2];
    za = tzmz[1] / tymy[1];
    zb = tzmz[2] / tymy[2];
    if (orient < 0) {
        swap(&ma, &mb, sizeof(float));
        swap(&za, &zb, sizeof(float));
    }

    int yB = -tymy[1];
    for (int y = y_end1; y < y_end2; y++) {
        const int padySB = y * wa.width;

        int x_start = (ma * yB) + txs[2];
        int x_end = (mb * yB) + txs[2];

        const float z1 = (za * yB) + tzs[2];
        const float z2 = (zb * yB) + tzs[2];

        const float xexs = x_end - x_start;
        const float z2z1 = z2 - z1;
        float xxs = 0.0;
        for (int x = x_start; x <= x_end; x++) {

            const float barycentric = xxs / xexs;
            const float depthZ = z1 + (barycentric * z2z1);

            const int padxSB = padySB + x;
            if ( depthZ < shadow_buffer[padxSB] ) {

                shadow_buffer[padxSB] = depthZ;
            }
            xxs += 1.0;
        }
        yB++;
    }
}
const int shadowTest(vec4f pixel) {
    vec4f r = pixel;
    r[3] = 1;
    /* Transform to Model space coordinates. */
    r = vecxm(r, lookAt);

    /* Transform to Light space coordinates. */
    r = vecxm(r, lightMat);

    /* Transform to Screen space coordinates. */
    r[0] = (1.0 + r[0]) * HALFW;
    if ( (r[0] < 0) || (r[0] >= wa.width) )
        return 0;

    r[1] = (1.0 + r[1]) * HALFH;
    if ( (r[1] < 0) || (r[1] >= wa.height) )
        return 0;

    r[2] *= 0.5;
    // printf("Shadow_buffer: %f    ", shadow_buffer[(int)((r[1] * wa.width) + r[0])]);
    // logVec4f(r);
    if ( r[2] > shadow_buffer[(int)((r[1] * wa.width) + r[0])] + shadow_bias)
        return 1;

    return 0;
}


