#include "headers/shadowmap.h"

extern XWindowAttributes wa;
extern int HALFW;
extern int HALFH;
extern float *shadow_buffer;
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
    vec4i xs = { f.v[0][0],  f.v[1][0], f.v[2][0], 0};
    vec4i ys = { f.v[0][1],  f.v[1][1], f.v[2][1], 0};
    vec4i xmx = xs - __builtin_shuffle(xs, mask);
    vec4i ymy = ys - __builtin_shuffle(ys, mask);

    vec4i txs = { f.v[srt[0].index][0], f.v[srt[1].index][0], f.v[srt[2].index][0], 0 };
    vec4i tys = { f.v[srt[0].index][1], f.v[srt[1].index][1], f.v[srt[2].index][1], 0 };
    vec4i txmx = __builtin_shuffle(txs, mask) - txs;
    vec4i tymy = __builtin_shuffle(tys, mask) - tys;

    const int tps0 = ((ymy[0] == 0) && (ys[2] < ys[1])) || (ymy[0] < 0) ? 1 : 0;
    const int tps1 = ((ymy[1] == 0) && (ys[0] < ys[2])) || (ymy[1] < 0) ? 1 : 0;
    const int tps2 = ((ymy[2] == 0) && (ys[1] < ys[0])) || (ymy[2] < 0) ? 1 : 0;

    const int orient = (txmx[0] * tymy[2]) - (tymy[0] * txmx[2]);
    float ma = (float)txmx[0] / tymy[0];
    float mb = (float)txmx[2] / tymy[2];
    if (orient < 0)
        swap(&ma, &mb, sizeof(float));

    const int y_start = tys[0];
    const int y_end1 = tys[1];
    const int y_end2 = tys[2];

    const float area = (xmx[0] * ymy[1]) - (ymy[0] * xmx[1]);
    vec4i ya = ((y_start - ys) * xmx);

    int yA = 0;
    if (tymy[0] != 0)
        for (int y = y_start; y < y_end1; y++) {
            const int padySB = y * wa.width;

            int x_start = (ma * yA) + txs[0];
            int x_end = (mb * yA) + txs[0];

            vec4i xa = ((x_start - xs) * ymy) - ya;

            for (int x = x_start; x <= x_end; x++) {

                const int xa0 = ( (tps0) && (!xa[0]) ) ? -1 : xa[0];
                const int xa1 = ( (tps1) && (!xa[1]) ) ? -1 : xa[1];
                const int xa2 = ( (tps2) && (!xa[2]) ) ? -1 : xa[2];

                if ( (xa0 | xa1 | xa2) > 0 ) {
                    vec4f a = __builtin_convertvector(xa, vec4f) / area;

                    const vec4f frag = a[0] * f.v[2] + a[1] * f.v[0] + a[2] * f.v[1];
                    const int padxDB = padySB + x;
                    if ( frag[2] < shadow_buffer[padxDB] ) {

                        shadow_buffer[padxDB] = frag[2];
                    }
                }
                xa += ymy;
            }
            ya += xmx;
            yA++;
        }

    if (tymy[1] == 0)
        return;

    ma = (float)txmx[1] / tymy[1];
    mb = (float)txmx[2] / tymy[2];
    if (orient < 0)
        swap(&ma, &mb, sizeof(float));

    int yB = -tymy[1];
    for (int y = y_end1; y < y_end2; y++) {
        const int padySB = y * wa.width;

        int x_start = (ma * yB) + txs[2];
        int x_end = (mb * yB) + txs[2];

        vec4i xa = ((x_start - xs) * ymy) - ya;

        for (int x = x_start; x <= x_end; x++) {

            const int xa0 = ( (tps0) && (!xa[0]) ) ? -1 : xa[0];
            const int xa1 = ( (tps1) && (!xa[1]) ) ? -1 : xa[1];
            const int xa2 = ( (tps2) && (!xa[2]) ) ? -1 : xa[2];

            if ( (xa0 | xa1 | xa2) > 0 ) {
                vec4f a = __builtin_convertvector(xa, vec4f) / area;

                const vec4f frag = a[0] * f.v[2] + a[1] * f.v[0] + a[2] * f.v[1];
                const int padxDB = padySB + x;
                if ( frag[2] < shadow_buffer[padxDB] ) {

                    shadow_buffer[padxDB] = frag[2];
                }
            }
            xa += ymy;
        }
        ya += xmx;
        yB++;
    }
}
const vec4f shadowTest(vec4f pixel) {
    vec4f r = pixel;
    r[3] = 1;
    /* Transform to Model space coordinates. */
    r = vecxm(r, lookAt);

    /* Transform to Light space coordinates. */
    r = vecxm(r, lightMat);

    /* Transform to Screen space coordinates. */
    r[0] = (1.0 + r[0]) * HALFW;
    if (r[0] >= wa.width)
        r[0] = wa.width - 1;
    else if (r[0] < 0)
        r[0] = 0;

    r[1] = (1.0 + r[1]) * HALFH;
    if (r[1] >= wa.height)
        r[1] = wa.height - 1;
    else if (r[1] < 0)
        r[1] = 0;

    r[2] *= 0.5;

    return r;
}

