#include "headers/draw_functions.h"

extern u_int8_t *frame_buffer;
extern float *depth_buffer;
extern XWindowAttributes wa;
extern const float winding(const face f);

const void drawLine(float x1, float y1, float x2, float y2, vec4f color) {
    color *= 255;
    /* Reversing rgb values here because it is like this in XLIB. (bgr) */
    u_int8_t rgba[4] = { color[2], color[1], color[0], color[3] };

    float delta_y = y2 - y1;
    float delta_x = x2 - x1;

    float fabsdy = fabs(delta_y);
    float fabsdx = fabs(delta_x);

    int start_y = y1 + 0.5;
    int end_y = y2 + 0.5;
    int start_x = x1 + 0.5;
    int end_x = x2 + 0.5;

    const int pad = wa.width * 4;
    int step_y, step_x;
    if ( (delta_x == 0) && (delta_y == 0) ) {
        return;
    } else if ( fabsdx >= fabsdy ) {
        float slope = delta_y / delta_x;

        if (delta_x < 0) {
            for (int x = start_x; x > end_x; x--) {
                step_y = ((slope * (x - start_x)) + y1);
                memcpy(&frame_buffer[(step_y * pad) + (x * 4)], &rgba, 4);
            }
        } else {
            for (int x = start_x; x < end_x; x++) {
                step_y = ((slope * (x - start_x)) + y1);
                memcpy(&frame_buffer[(step_y * pad) + (x * 4)], &rgba, 4);
            }
        }
    } else if ( fabsdx < fabsdy ) {
        float slope = delta_x / delta_y;

        if (delta_y < 0) {
            for (int y = start_y; y > end_y; y--) {
                step_x = ((slope * (y - start_y)) + x1);
                memcpy(&frame_buffer[(y * pad) + (step_x * 4)], &rgba, 4);
            }
        } else {
            for (int y = start_y; y < end_y; y++) {
                step_x = ((slope * (y - start_y)) + x1);
                memcpy(&frame_buffer[(y * pad) + (step_x * 4)], &rgba, 4);
            }
        }
    } else {
        fprintf(stderr, "An Error has occured! draw_line().");
        exit(EXIT_FAILURE);
    }
}
/* Fills given mesh with color according to mesh material. */
const void fillMesh(const Mesh m) {
    for (int i = 0; i < m.f_indexes; i++) {
        fillface(m.f[i], m.material);
    }
}
const void fillface(const face f, const Material mtr) {
    /* Creating 2Arrays for X and Y values to sort them. */
    int Ys[3] = { f.v[0][1], f.v[1][1], f.v[2][1] };
    int Xs[3] = { f.v[0][0], f.v[1][0], f.v[2][0] };
    /* Sorting the values from smaller to larger. Those values are the triangle bounding box. */
    float temp;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            if (Ys[i] < Ys[j]) {

                temp = Ys[j];
                Ys[j] = Ys[i];
                Ys[i] = temp;
            }
            if (Xs[i] < Xs[j]) {

                temp = Xs[j];
                Xs[j] = Xs[i];
                Xs[i] = temp;
            }
        }

    fillGeneral(f, mtr, Xs[0], Xs[2], Ys[0], Ys[2]);
}
const static void fillGeneral(const face f, const Material mtr, int minX, int maxX, int minY, int maxY) {
    vec4i xs = { f.v[0][0],  f.v[1][0], f.v[2][0], 0};
    vec4i ys = { f.v[0][1],  f.v[1][1], f.v[2][1], 0};

    vec4i mask = { 1, 2, 0, 3 };
    vec4i sflx = __builtin_shuffle(xs, mask);
    vec4i sfly = __builtin_shuffle(ys, mask);

    vec4i xmx = xs - sflx;
    vec4i ymy = ys - sfly;

    vec4i tps = { 0 };
    tps[0] = ((ymy[0] == 0) && (ys[2] > ys[1])) || (ymy[0] < 0) ? 1 : 0;
    tps[1] = ((ymy[1] == 0) && (ys[0] > ys[2])) || (ymy[1] < 0) ? 1 : 0;
    tps[2] = ((ymy[2] == 0) && (ys[1] > ys[0])) || (ymy[2] < 0) ? 1 : 0;

    const float area = ((xs[0] - xs[1]) * ymy[1]) - ((ys[0] - ys[1]) * xmx[1]);
    vec4i ya = ((minX - xs) * ymy) - ((minY - ys) * xmx);

    for (int y = minY; y <= maxY; y++) {

        vec4i xa = ya;
        int xflag = 0;
        const int padyDB = y * wa.width;
        const int padyFB = padyDB * 4;
        for (int x = minX; x <= maxX; x++) {

            const int xa0 = ( (tps[0]) && (!xa[0]) ) ? -1 : xa[0];
            const int xa1 = ( (tps[1]) && (!xa[1]) ) ? -1 : xa[1];
            const int xa2 = ( (tps[2]) && (!xa[2]) ) ? -1 : xa[2];

            if ( (xa0 | xa1 | xa2) > 0 ) {
                vec4f a = __builtin_convertvector(xa, vec4f);
                a /= area;

                const vec4f frag = a[0] * f.v[2] + a[1] * f.v[0] + a[2] * f.v[1];

                if (frag[3] > depth_buffer[padyDB + x]) {

                    const vec4f normal = a[0] * f.vn[2] + a[1] * f.vn[0] + a[2] * f.vn[1];

                    depth_buffer[padyDB + x] = phong(normal, mtr, x, y, frag[2], frag[3]);
                }
                xflag++;
            } else if (xflag) break;
            xa += ymy;
        }
        ya += -xmx;
    }
}


