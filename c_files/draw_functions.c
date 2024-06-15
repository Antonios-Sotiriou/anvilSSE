#include "../headers/draw_functions.h"

/* Global masks. Probably must be moved to a global variables file. */
const vec4i edgemask = { 1, 2, 0, 3 };
const vec4i rgbmask = { 2, 1, 0, 3 };

const static void edgefillGeneral(const face f, Material *mtr, const int minX, const int maxX, const int minY, const int maxY);
const static void scanlinefillGeneral(const face f, Material *mtr, const Srt srt[]);

const static void edgetexGeneral(face *f, Material *mtr, const int minX, const int maxX, const int minY, const int maxY);
const static void scanlinetexGeneral(face *f, Material *mtr, const Srt srt[]);


const void drawLine(float x1, float y1, float x2, float y2, vec4f color) {
    /* Reversing rgb values here because it is like this in XLIB. (bgr) */
    vec4c rgba = __builtin_convertvector(__builtin_shuffle(color * 255, rgbmask), vec4c);

    float delta_y = y2 - y1;
    float delta_x = x2 - x1;

    float fabsdy = fabs(delta_y);
    float fabsdx = fabs(delta_x);

    int start_y = y1 + 0.5;
    int end_y = y2 + 0.5;
    int start_x = x1 + 0.5;
    int end_x = x2 + 0.5;

    const int pad = point_attrib->width * 4;
    int step_y, step_x;
    if ( (delta_x == 0) && (delta_y == 0) ) {
        return;
    } else if ( fabsdx >= fabsdy ) {
        float slope = delta_y / delta_x;

        if (delta_x < 0) {
            for (int x = start_x; x > end_x; x--) {
                step_y = ((slope * (x - start_x)) + y1);
                memcpy(&point_buffer[(step_y * pad) + (x * 4)], &rgba, 4);
            }
        } else {
            for (int x = start_x; x < end_x; x++) {
                step_y = ((slope * (x - start_x)) + y1);
                memcpy(&point_buffer[(step_y * pad) + (x * 4)], &rgba, 4);
            }
        }
    } else if ( fabsdx < fabsdy ) {
        float slope = delta_x / delta_y;

        if (delta_y < 0) {
            for (int y = start_y; y > end_y; y--) {
                step_x = ((slope * (y - start_y)) + x1);
                memcpy(&point_buffer[(y * pad) + (step_x * 4)], &rgba, 4);
            }
        } else {
            for (int y = start_y; y < end_y; y++) {
                step_x = ((slope * (y - start_y)) + x1);
                memcpy(&point_buffer[(y * pad) + (step_x * 4)], &rgba, 4);
            }
        }
    } else {
        fprintf(stderr, "An Error has occured! draw_line().");
        exit(EXIT_FAILURE);
    }
}
/* Draws face edges of given mesh with color. */
const void edgeMesh(MeshStepTwo *m, const vec4f color) {
    for (int i = 0; i < m->f_indexes; i++) {
        drawLine(m->f[i].v[0][0], m->f[i].v[0][1], m->f[i].v[1][0], m->f[i].v[1][1], color);
        drawLine(m->f[i].v[1][0], m->f[i].v[1][1], m->f[i].v[2][0], m->f[i].v[2][1], color);
        drawLine(m->f[i].v[2][0], m->f[i].v[2][1], m->f[i].v[0][0], m->f[i].v[0][1], color);
    }
}
/* ######################################## FILLING FUNCTIONS ######################################## */

/* Fills given mesh with color according to mesh material und with the appropriate fill method. */
const void fillMesh(MeshStepTwo *m, Material *mtr) {
    struct fillDispatch {
        void (*fillfunction)(face, Material *m);
    } dis;

    if (EDGEFUNC)
        dis.fillfunction = &edgefillface;
    else if (SCANLINE)
        dis.fillfunction = &scanlinefillface;

    for (int i = 0; i < m->f_indexes; i++) {
        dis.fillfunction(m->f[i], mtr);
    }
}
const void edgefillface(const face f, Material *mtr) {
    /* Creating 2Arrays for X and Y values to sort them-> */
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

    edgefillGeneral(f, mtr, Xs[0], Xs[2], Ys[0], Ys[2]);
}
const static void edgefillGeneral(const face f, Material *mtr, int minX, int maxX, int minY, int maxY) {
    const vec4i xs = { f.v[0][0],  f.v[1][0], f.v[2][0], 0};
    const vec4i ys = { f.v[0][1],  f.v[1][1], f.v[2][1], 0};

    const vec4i xmx = xs - __builtin_shuffle(xs, edgemask);
    const vec4i ymy = ys - __builtin_shuffle(ys, edgemask);

    const int tps0 = ((ymy[0] == 0) && (ys[2] > ys[1])) || (ymy[0] < 0) ? 1 : 0;
    const int tps1 = ((ymy[1] == 0) && (ys[0] > ys[2])) || (ymy[1] < 0) ? 1 : 0;
    const int tps2 = ((ymy[2] == 0) && (ys[1] > ys[0])) || (ymy[2] < 0) ? 1 : 0;

    const float area = ((xs[0] - xs[1]) * ymy[1]) - ((ys[0] - ys[1]) * xmx[1]);
    vec4i ya = ((minX - xs) * ymy) - ((minY - ys) * xmx);

    for (int y = minY; y <= maxY; y++) {

        vec4i xa = ya;
        int xflag = 0;
        const int padyDB = y * point_attrib->width;
        for (int x = minX; x <= maxX; x++) {

            const int xa0 = ( (tps0) && (!xa[0]) ) ? -1 : xa[0];
            const int xa1 = ( (tps1) && (!xa[1]) ) ? -1 : xa[1];
            const int xa2 = ( (tps2) && (!xa[2]) ) ? -1 : xa[2];

            if ( (xa0 | xa1 | xa2) > 0 ) {
                vec4f a = __builtin_convertvector(xa, vec4f) / area;

                const vec4f frag = a[0] * f.v[2] + a[1] * f.v[0] + a[2] * f.v[1];
                const int padxDB = padyDB + x;
                if (frag[3] > point_depth_buffer[padxDB]) {

                    const vec4f normal = a[0] * f.vn[2] + a[1] * f.vn[0] + a[2] * f.vn[1];

                    vec4f temp = { x, y, frag[2], frag[3] };
                    frags_buffer[padxDB].pos = temp;
                    frags_buffer[padxDB].nrm = normal;
                    frags_buffer[padxDB].mtr = mtr;
                    frags_buffer[padxDB].state = 1;
                    point_depth_buffer[padxDB] = frag[3];
                }
                xflag++;
            } else if (xflag) break;
            xa += ymy;
        }
        ya += -xmx;
    }
}
const void scanlinefillface(const face f, Material *mtr) {
    /* Creating a struct to be able to sort vertices without affecting their order. */
    Srt srt[3] = {
        { .y =  f.v[0][1], .index = 0},
        { .y =  f.v[1][1], .index = 1},
        { .y =  f.v[2][1], .index = 2}
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

    scanlinefillGeneral(f, mtr, srt);
}
const static void scanlinefillGeneral(const face f, Material *mtr, const Srt srt[]) {
    const vec4i xs = { f.v[0][0],  f.v[1][0], f.v[2][0], 0};
    const vec4i ys = { f.v[0][1],  f.v[1][1], f.v[2][1], 0};
    const vec4i xmx = xs - __builtin_shuffle(xs, edgemask);
    const vec4i ymy = ys - __builtin_shuffle(ys, edgemask);

    const vec4i txs = { f.v[srt[0].index][0], f.v[srt[1].index][0], f.v[srt[2].index][0], 0 };
    const vec4i tys = { f.v[srt[0].index][1], f.v[srt[1].index][1], f.v[srt[2].index][1], 0 };
    const vec4i txmx = __builtin_shuffle(txs, edgemask) - txs;
    const vec4i tymy = __builtin_shuffle(tys, edgemask) - tys;

    const int tps0 = ((ymy[0] == 0) && (ys[2] < ys[1])) || (ymy[0] < 0) ? 1 : 0;
    const int tps1 = ((ymy[1] == 0) && (ys[0] < ys[2])) || (ymy[1] < 0) ? 1 : 0;
    const int tps2 = ((ymy[2] == 0) && (ys[1] < ys[0])) || (ymy[2] < 0) ? 1 : 0;

    const int orient = (txmx[0] * tymy[2]) - (tymy[0] * txmx[2]);
    float ma = (float)txmx[0] / tymy[0];
    float mb = (float)txmx[2] / tymy[2];
    if (orient < 0)
        swap(&ma, &mb, 4);

    const int y_start = tys[0];
    const int y_end1 = tys[1];
    const int y_end2 = tys[2];

    const float area = (xmx[0] * ymy[1]) - (ymy[0] * xmx[1]);
    vec4i ya = ((y_start - ys) * xmx);

    int yA = 0;
    if (tymy[0] != 0)
        for (int y = y_start; y < y_end1; y++) {
            const int padyDB = y * point_attrib->width;

            const int x_start = (ma * yA) + txs[0];
            const int x_end = (mb * yA) + txs[0];

            vec4i xa = ((x_start - xs) * ymy) - ya;

            for (int x = x_start; x <= x_end; x++) {

                const int xa0 = ( (tps0) && (!xa[0]) ) ? -1 : xa[0];
                const int xa1 = ( (tps1) && (!xa[1]) ) ? -1 : xa[1];
                const int xa2 = ( (tps2) && (!xa[2]) ) ? -1 : xa[2];

                if ( (xa0 | xa1 | xa2) > 0 ) {
                    vec4f a = __builtin_convertvector(xa, vec4f) / area;

                    const vec4f frag = a[0] * f.v[2] + a[1] * f.v[0] + a[2] * f.v[1];
                    const int padxDB = padyDB + x;
                    if ( frag[3] > point_depth_buffer[padxDB] ) {

                        const vec4f normal = a[0] * f.vn[2] + a[1] * f.vn[0] + a[2] * f.vn[1];

                        vec4f temp = { x, y, frag[2], frag[3] };
                        frags_buffer[padxDB].pos = temp;
                        frags_buffer[padxDB].nrm = normal;
                        frags_buffer[padxDB].mtr = mtr;
                        frags_buffer[padxDB].state = 1;
                        point_depth_buffer[padxDB] = frag[3];
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
        swap(&ma, &mb, 4);

    int yB = -tymy[1];
    for (int y = y_end1; y < y_end2; y++) {
        const int padyDB = y * point_attrib->width;

        const int x_start = (ma * yB) + txs[2];
        const int x_end = (mb * yB) + txs[2];

        vec4i xa = ((x_start - xs) * ymy) - ya;

        for (int x = x_start; x <= x_end; x++) {

            const int xa0 = ( (tps0) && (!xa[0]) ) ? -1 : xa[0];
            const int xa1 = ( (tps1) && (!xa[1]) ) ? -1 : xa[1];
            const int xa2 = ( (tps2) && (!xa[2]) ) ? -1 : xa[2];

            if ( (xa0 | xa1 | xa2) > 0 ) {
                vec4f a = __builtin_convertvector(xa, vec4f) / area;

                const vec4f frag = a[0] * f.v[2] + a[1] * f.v[0] + a[2] * f.v[1];
                const int padxDB = padyDB + x;
                if ( frag[3] > point_depth_buffer[padxDB] ) {

                    const vec4f normal = a[0] * f.vn[2] + a[1] * f.vn[0] + a[2] * f.vn[1];

                    vec4f temp = { x, y, frag[2], frag[3] };
                    frags_buffer[padxDB].pos = temp;
                    frags_buffer[padxDB].nrm = normal;
                    frags_buffer[padxDB].mtr = mtr;
                    frags_buffer[padxDB].state = 1;
                    point_depth_buffer[padxDB] = frag[3];
                }
            }
            xa += ymy;
        }
        ya += xmx;
        yB++;
    }
}
/* ######################################## TEXTURE PAINTING FUNCTIONS ######################################## */

/* Textures given Mesh using the choosen algorithm-> (Edge Function or Scanline Function). */
const void texMesh(MeshStepTwo *m, Material *mtr) {
    struct fillDispatch {
        void (*fillfunction)(face *f, Material *m);
    } dis;

    if (EDGEFUNC)
        dis.fillfunction = &edgetexface;
    else if (SCANLINE)
        dis.fillfunction = &scanlinetexface;

    for (int i = 0; i < m->f_indexes; i++) {
        dis.fillfunction(&m->f[i], mtr);
    }
}
const void edgetexface(face *f, Material *mtr) {
    /* Creating 2Arrays for X and Y values to sort them-> */
    int Ys[3] = { f->v[0][1], f->v[1][1], f->v[2][1] };
    int Xs[3] = { f->v[0][0], f->v[1][0], f->v[2][0] };
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

    edgetexGeneral(f, mtr, Xs[0], Xs[2], Ys[0], Ys[2]);
}
const static void edgetexGeneral(face *f, Material *mtr, int minX, int maxX, int minY, int maxY) {
    const vec4i xs = { f->v[0][0],  f->v[1][0], f->v[2][0], 0};
    const vec4i ys = { f->v[0][1],  f->v[1][1], f->v[2][1], 0};

    const vec4i xmx = xs - __builtin_shuffle(xs, edgemask);
    const vec4i ymy = ys - __builtin_shuffle(ys, edgemask);

    const int tps0 = ((ymy[0] == 0) && (ys[2] > ys[1])) || (ymy[0] < 0) ? 1 : 0;
    const int tps1 = ((ymy[1] == 0) && (ys[0] > ys[2])) || (ymy[1] < 0) ? 1 : 0;
    const int tps2 = ((ymy[2] == 0) && (ys[1] > ys[0])) || (ymy[2] < 0) ? 1 : 0;

    const float area = ((xs[0] - xs[1]) * ymy[1]) - ((ys[0] - ys[1]) * xmx[1]);
    vec4i ya = ((minX - xs) * ymy) - ((minY - ys) * xmx);

    for (int y = minY; y <= maxY; y++) {

        vec4i xa = ya;
        int xflag = 0;
        const int padyDB = y * point_attrib->width;
        for (int x = minX; x <= maxX; x++) {

            const int xa0 = ( (tps0) && (!xa[0]) ) ? -1 : xa[0];
            const int xa1 = ( (tps1) && (!xa[1]) ) ? -1 : xa[1];
            const int xa2 = ( (tps2) && (!xa[2]) ) ? -1 : xa[2];

            if ( (xa0 | xa1 | xa2) > 0 ) {
                vec4f a = __builtin_convertvector(xa, vec4f) / area;

                const vec4f frag = a[0] * f->v[2] + a[1] * f->v[0] + a[2] * f->v[1];
                const int padxDB = padyDB + x;
                if (frag[3] > point_depth_buffer[padxDB]) {

                    const vec4f normal = a[0] * f->vn[2] + a[1] * f->vn[0] + a[2] * f->vn[1];

                    if (mtr->texlevels) {
                        const vec2f texel = a[0] * f->vt[2] + a[1] * f->vt[0] + a[2] * f->vt[1];

                        frags_buffer[padxDB].tex_y = (texel[1] * (mtr->texture_height - 1)) / frag[3];
                        frags_buffer[padxDB].tex_x = (texel[0] * (mtr->texture_width - 1)) / frag[3];
                    }
                    vec4f temp = { x, y, frag[2], frag[3] };
                    frags_buffer[padxDB].pos = temp;
                    frags_buffer[padxDB].nrm = normal;
                    frags_buffer[padxDB].mtr = mtr;
                    frags_buffer[padxDB].state = 1;
                    point_depth_buffer[padxDB] = frag[3];
                }
                xflag++;
            } else if (xflag) break;
            xa += ymy;
        }
        ya += -xmx;
    }
}
const void scanlinetexface(face *f, Material *mtr) {
    /* Creating a struct to be able to sort vertices without affecting their order. */
    Srt srt[3] = {
        { .y =  f->v[0][1], .index = 0},
        { .y =  f->v[1][1], .index = 1},
        { .y =  f->v[2][1], .index = 2}
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

    scanlinetexGeneral(f, mtr, srt);
}
const static void scanlinetexGeneral(face *f, Material *mtr, const Srt srt[]) {
    const vec4i xs = { f->v[0][0],  f->v[1][0], f->v[2][0], 0};
    const vec4i ys = { f->v[0][1],  f->v[1][1], f->v[2][1], 0};
    const vec4i xmx = xs - __builtin_shuffle(xs, edgemask);
    const vec4i ymy = ys - __builtin_shuffle(ys, edgemask);

    const vec4i txs = { f->v[srt[0].index][0], f->v[srt[1].index][0], f->v[srt[2].index][0], 0 };
    const vec4i tys = { f->v[srt[0].index][1], f->v[srt[1].index][1], f->v[srt[2].index][1], 0 };
    const vec4i txmx = __builtin_shuffle(txs, edgemask) - txs;
    const vec4i tymy = __builtin_shuffle(tys, edgemask) - tys;

    const int tps0 = ((ymy[0] == 0) && (ys[2] < ys[1])) || (ymy[0] < 0) ? 1 : 0;
    const int tps1 = ((ymy[1] == 0) && (ys[0] < ys[2])) || (ymy[1] < 0) ? 1 : 0;
    const int tps2 = ((ymy[2] == 0) && (ys[1] < ys[0])) || (ymy[2] < 0) ? 1 : 0;

    const int orient = (txmx[0] * tymy[2]) - (tymy[0] * txmx[2]);
    float ma = (float)txmx[0] / tymy[0];
    float mb = (float)txmx[2] / tymy[2];
    if (orient < 0) {
        swap(&ma, &mb, 4);
    }

    const int y_start = tys[0];
    const int y_end1 = tys[1];
    const int y_end2 = tys[2];

    const float area = (xmx[0] * ymy[1]) - (ymy[0] * xmx[1]);
    vec4i ya = ((y_start - ys) * xmx);

    int yA = 0;
    if (tymy[0] != 0)
        for (int y = y_start; y < y_end1; y++) {
            const int padyDB = y * point_attrib->width;

            const int x_start = (ma * yA) + txs[0];
            const int x_end = (mb * yA) + txs[0];

            vec4i xa = ((x_start - xs) * ymy) - ya;

            for (int x = x_start; x <= x_end; x++) {

                const int xa0 = ( (tps0) && (!xa[0]) ) ? -1 : xa[0];
                const int xa1 = ( (tps1) && (!xa[1]) ) ? -1 : xa[1];
                const int xa2 = ( (tps2) && (!xa[2]) ) ? -1 : xa[2];

                if ( (xa0 | xa1 | xa2) > 0 ) {
                    vec4f a = __builtin_convertvector(xa, vec4f) / area;

                    const vec4f frag = a[0] * f->v[2] + a[1] * f->v[0] + a[2] * f->v[1];
                    const int padxDB = padyDB + x;
                    if ( frag[3] > point_depth_buffer[padxDB] ) {

                        const vec4f normal = a[0] * f->vn[2] + a[1] * f->vn[0] + a[2] * f->vn[1];

                        if (mtr->texlevels) {
                            const vec2f texel = a[0] * f->vt[2] + a[1] * f->vt[0] + a[2] * f->vt[1];

                            frags_buffer[padxDB].tex_y = (texel[1] * (mtr->texture_height - 1)) / frag[3];
                            frags_buffer[padxDB].tex_x = (texel[0] * (mtr->texture_width - 1)) / frag[3];
                        }
                        vec4f temp = { x, y, frag[2], frag[3] };
                        frags_buffer[padxDB].pos = temp;
                        frags_buffer[padxDB].nrm = normal;
                        frags_buffer[padxDB].mtr = mtr;
                        frags_buffer[padxDB].state = 1;
                        point_depth_buffer[padxDB] = frag[3];
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
    if (orient < 0) {
        swap(&ma, &mb, 4);
    }

    int yB = -tymy[1];
    for (int y = y_end1; y < y_end2; y++) {
        const int padyDB = y * point_attrib->width;

        const int x_start = (ma * yB) + txs[2];
        const int x_end = (mb * yB) + txs[2];

        vec4i xa = ((x_start - xs) * ymy) - ya;

        for (int x = x_start; x <= x_end; x++) {

            const int xa0 = ( (tps0) && (!xa[0]) ) ? -1 : xa[0];
            const int xa1 = ( (tps1) && (!xa[1]) ) ? -1 : xa[1];
            const int xa2 = ( (tps2) && (!xa[2]) ) ? -1 : xa[2];

            if ( (xa0 | xa1 | xa2) > 0 ) {
                vec4f a = __builtin_convertvector(xa, vec4f) / area;

                const vec4f frag = a[0] * f->v[2] + a[1] * f->v[0] + a[2] * f->v[1];
                const int padxDB = padyDB + x;
                if ( frag[3] > point_depth_buffer[padxDB] ) {

                    const vec4f normal = a[0] * f->vn[2] + a[1] * f->vn[0] + a[2] * f->vn[1];

                    if (mtr->texlevels) {
                        const vec2f texel = a[0] * f->vt[2] + a[1] * f->vt[0] + a[2] * f->vt[1];
                        
                        frags_buffer[padxDB].tex_y = (texel[1] * (mtr->texture_height - 1)) / frag[3];
                        frags_buffer[padxDB].tex_x = (texel[0] * (mtr->texture_width - 1)) / frag[3];
                    }
                    vec4f temp = { x, y, frag[2], frag[3] };
                    frags_buffer[padxDB].pos = temp;
                    frags_buffer[padxDB].nrm = normal;
                    frags_buffer[padxDB].mtr = mtr;
                    frags_buffer[padxDB].state = 1;
                    point_depth_buffer[padxDB] = frag[3];
                }
            }
            xa += ymy;
        }
        ya += xmx;
        yB++;
    }
}


