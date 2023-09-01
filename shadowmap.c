#include "headers/shadowmap.h"

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
    vec4i xs = { f.v[srt[0].index][0], f.v[srt[1].index][0], f.v[srt[2].index][0], 0 };
    vec4i ys = { f.v[srt[0].index][1], f.v[srt[1].index][1], f.v[srt[2].index][1], 0 };
    vec4f zs = { f.v[srt[0].index][2], f.v[srt[1].index][2], f.v[srt[2].index][2], 0 };
    vec4i xmx = __builtin_shuffle(xs, mask) - xs;
    vec4i ymy = __builtin_shuffle(ys, mask) - ys;
    vec4f zmz = __builtin_shuffle(zs, mask) - zs;

    const int orient = (xmx[0] * ymy[2]) - (ymy[0] * xmx[2]);
    float ma = (float)xmx[0] / ymy[0];
    float mb = (float)xmx[2] / ymy[2];
    float za = zmz[0] / ymy[0];
    float zb = zmz[2] / ymy[2];
    if (orient < 0) {
        swap(&ma, &mb, sizeof(float));
        swap(&za, &zb, sizeof(float));
    }

    const int y_start = ys[0];
    const int y_end1 = ys[1];
    const int y_end2 = ys[2];

    int yA = 0;
    if (ymy[0] != 0)
        for (int y = y_start; y < y_end1; y++) {
            const int padySB = y * wa.width;

            int x_start = (ma * yA) + xs[0];
            int x_end = (mb * yA) + xs[0];

            const float z1 = (za * yA) + zs[0];
            const float z2 = (zb * yA) + zs[0];

            const float xexs = x_end - x_start;
            const float z2z1 = z2 - z1;
            float xxs = 0.0;
            for (int x = x_start; x <= x_end; x++) {

                const float barycentric = xxs / xexs;
                const float depthZ = z1 + (barycentric * z2z1);

                const int padxSB = padySB + x;
                if ( depthZ < shadow_buffer[padxSB] ) {

                    shadow_buffer[padxSB] = depthZ;
                } else if (depthZ == shadow_buffer[padxSB]) {
                    shadow_buffer[padxSB] = 0.f;
                }
                xxs += 1.0;
            }
            yA++;
        }

    if (ymy[1] == 0)
        return;

    ma = (float)xmx[1] / ymy[1];
    mb = (float)xmx[2] / ymy[2];
    za = zmz[1] / ymy[1];
    zb = zmz[2] / ymy[2];
    if (orient < 0) {
        swap(&ma, &mb, sizeof(float));
        swap(&za, &zb, sizeof(float));
    }

    int yB = -ymy[1];
    for (int y = y_end1; y < y_end2; y++) {
        const int padySB = y * wa.width;

        int x_start = (ma * yB) + xs[2];
        int x_end = (mb * yB) + xs[2];

        const float z1 = (za * yB) + zs[2];
        const float z2 = (zb * yB) + zs[2];

        const float xexs = x_end - x_start;
        const float z2z1 = z2 - z1;
        float xxs = 0.0;
        for (int x = x_start; x <= x_end; x++) {

            const float barycentric = xxs / xexs;
            const float depthZ = z1 + (barycentric * z2z1);

            const int padxSB = padySB + x;
            if ( depthZ < shadow_buffer[padxSB] ) {

                shadow_buffer[padxSB] = depthZ;
            } else if (depthZ == shadow_buffer[padxSB]) {
                shadow_buffer[padxSB] = 0.f;
            }
            xxs += 1.0;
        }
        yB++;
    }
}
const int shadowTest(vec4f pixel) {
    vec4f r = pixel;
    r[3] = 1.f;
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
    if ( r[2] > shadow_buffer[((int)(r[1]) * wa.width) + (int)(r[0])] + shadow_bias)
        return 1;

    return 0;
}


