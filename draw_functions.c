#include "headers/draw_functions.h"

const void checkQuad(int xa, int xb, int xc, 
                     int y10, int y21, int y02, 
                     float area, 
                     float z0, float z1, float z2, 
                     float w0, float w1, float w2, 
                     int x, int y, 
                     int tpA, int tpB, int tpC, 
                     int modulo,
                     Triangle t)
{
    v128i edgesi[modulo], step_x;
    step_x.mm = _mm_set_epi32(0, y02, y21, y10);
    edgesi[0].mm = _mm_set_epi32(0, xc, xb, xa);
    int masks[modulo];

    for (int i = 0; i < modulo; i++) {
        if (i > 0)
            edgesi[i].mm = _mm_add_epi32(edgesi[i - 1].mm, step_x.mm);

        edgesi[i].i32[0] = (tpA && !edgesi[i].i32[0]) ? -1 : edgesi[i].i32[0];
        edgesi[i].i32[1] = (tpB && !edgesi[i].i32[1]) ? -1 : edgesi[i].i32[1];
        edgesi[i].i32[2] = (tpC && !edgesi[i].i32[2]) ? -1 : edgesi[i].i32[2];

        masks[i] = (edgesi[i].i32[0] | edgesi[i].i32[1] | edgesi[i].i32[2]) < 0 ? -1 : 1;
    }

    if ( masks[0] < 0 && masks[1] < 0 && masks[2] < 0 && masks[3] < 0 )
        return;

    v128f ar = { .mm = _mm_set1_ps(area) };
    v128f edgesf[modulo];

    for (int i = 0; i < modulo; i++){

        if ( (edgesi[i].i32[0] | edgesi[i].i32[1] | edgesi[i].i32[2]) > 0 ) {
            edgesf[i].mm = _mm_div_ps(_mm_cvtepi32_ps(edgesi[i].mm), ar.mm);

            v128f az = { .mm = _mm_mul_ps(edgesf[i].mm, _mm_set_ps(0.0, z1, z0, z2)) };
            v128f aw = { .mm = _mm_mul_ps(edgesf[i].mm, _mm_set_ps(0.0, w1, w0, w2)) };

            const float depthZ = az.f32[0] + az.f32[1] + az.f32[2];
            const float depthW = aw.f32[0] + aw.f32[1] + aw.f32[2];

            if ( depthW > depth_buffer[y][x] ) {
                v128f normx = { .mm = _mm_mul_ps(_mm_set_ps(0.0, t.vn[1].x, t.vn[0].x, t.vn[2].x), edgesf[i].mm) };
                v128f normy = { .mm = _mm_mul_ps(_mm_set_ps(0.0, t.vn[1].y, t.vn[0].y, t.vn[2].y), edgesf[i].mm) };
                v128f normz = { .mm = _mm_mul_ps(_mm_set_ps(0.0, t.vn[1].z, t.vn[0].z, t.vn[2].z), edgesf[i].mm) };
                model.normal.x = normx.f32[0] + normx.f32[1] + normx.f32[2];
                model.normal.y = normy.f32[0] + normy.f32[1] + normy.f32[2];
                model.normal.z = normz.f32[0] + normz.f32[1] + normz.f32[2];

                depth_buffer[y][x] = phong(model, x, y, depthZ, depthW);
            }
        }
        x++;
    }
}
const Pixel antialliasing(const Pixel a, const Pixel b) {
    Pixel r = { 0 };
    r.Red = (a.Red + b.Red) * 0.5;
    r.Green = (a.Green + b.Green) * 0.5;
    r.Blue = (a.Blue + b.Blue) * 0.5;
    return r;
}

const void drawLine(float x1, float y1, float x2, float y2, const float red, const float green, const float blue) {
    Pixel pix = { blue, green, red }, s1, s2;
    Pixel test1 = { 255, 255, 255 };
    Pixel test2 = { 255, 0, 0 };
    float delta_y = y2 - y1;
    float delta_x = x2 - x1;

    float fabsdy = fabs(delta_y);
    float fabsdx = fabs(delta_x);

    int start_y = y1 + 0.5;
    int end_y = y2 + 0.5;
    int start_x = x1 + 0.5;
    int end_x = x2 + 0.5;

    int step_y, step_x;
    int step_cache = start_y;
    if ( (delta_x == 0) && (delta_y == 0) ) {
        memcpy(&pixels[start_y][start_x], &pix, sizeof(Pixel));
    } else if ( fabsdx >= fabsdy ) {
        float slope = delta_y / delta_x;

        if (delta_x < 0) {

            for (int x = start_x; x > end_x; x--) {
                step_y = (slope * (x - start_x)) + y1;
                memcpy(&pixels[step_y][x], &pix, sizeof(Pixel));
                if ( step_cache != step_y) {
                    s1 = antialliasing(pixels[step_y][x], pixels[step_cache][x]);
                    s2 = antialliasing(pixels[step_y][x], pixels[step_y][x + 1]);
                    memcpy(&pixels[step_cache][x], &test1, sizeof(Pixel));
                    memcpy(&pixels[step_y][x + 1], &test2, sizeof(Pixel));
                }
                step_cache = step_y;
            }
        } else {
            int step_cache;
            for (int x = start_x; x < end_x; x++) {
                step_y = (slope * (x - start_x)) + y1;
                memcpy(&pixels[step_y][x], &pix, sizeof(Pixel));
                step_cache = step_y;
            }
        }
    } else if ( fabsdx < fabsdy ) {
        float slope = delta_x / delta_y;

        if (delta_y < 0) {

            for (int y = start_y; y > end_y; y--) {
                step_x = (slope * (y - start_y)) + x1;
                memcpy(&pixels[y][step_x], &pix, sizeof(Pixel));
            }
        } else {

            for (int y = start_y; y < end_y; y++) {
                step_x = (slope * (y - start_y)) + x1;
                memcpy(&pixels[y][step_x], &pix, sizeof(Pixel));
            }
        }
    } else {
        fprintf(stderr, "An Error has occured! draw_line().");
        exit(EXIT_FAILURE);
    }
}
const void fillTriangle(const Triangle t) {
    /* Creating 2Arrays for X and Y values to sort them. */
    float Ys[3] = { t.v[0].y, t.v[1].y, t.v[2].y };
    float Xs[3] = { t.v[0].x, t.v[1].x, t.v[2].x };
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

    model.normal = t.fn;
    float winding = 1 / winding3D(t);
    model.bias = (winding <= 0.000026 && winding >= 0.0) ? 0.0017 : 0.0009;
    fillGeneral(t, Xs[0] + 0.5, Xs[2] + 0.5, Ys[0] + 0.5, Ys[2] + 0.5);
}
const void fillGeneral(const Triangle t, int minX, int maxX, int minY, int maxY) {
    const int maxHeight = wa.height - 1;
    const int maxWidth = wa.width - 1;
    const int x0 = t.v[0].x + 0.5,    x1 = t.v[1].x + 0.5,    x2 = t.v[2].x + 0.5;
    const int y0 = t.v[0].y + 0.5,    y1 = t.v[1].y + 0.5,    y2 = t.v[2].y + 0.5;
    const float z0 = t.v[0].z,    z1 = t.v[1].z,     z2 = t.v[2].z;
    const float w0 = t.v[0].w,    w1 = t.v[1].w,     w2 = t.v[2].w;
    int x10 = x0 - x1,    x20 = x2 - x0,    x02 = x2 - x0,    x21 = x1 - x2;
    int y10 = y0 - y1,    y20 = y2 - y0,    y02 = y2 - y0,    y21 = y1 - y2;

    const int tpA = ((y10 == 0) && (t.v[2].y > t.v[1].y)) || (y10 < 0) ? 1 : 0;
    const int tpB = ((y21 == 0) && (t.v[0].y > t.v[2].y)) || (y21 < 0) ? 1 : 0;
    const int tpC = ((y02 == 0) && (t.v[1].y > t.v[0].y)) || (y02 < 0) ? 1 : 0;

    const int area = ((x0 - x1) * y21) - ((y0 - y1) * x21);
    int ya = ((minX - x0) * y10) - ((minY - y0) * x10);
    int yb = ((minX - x1) * y21) - ((minY - y1) * x21);
    int yc = ((minX - x2) * y02) - ((minY - y2) * x02);

    const int modulo = maxX % 4;
    const int limit = maxX - modulo;
    for (int y = minY; y <= maxY; y++) {
        int xa = ya;
        int xb = yb;
        int xc = yc;
        for (int x = minX; x <= maxX; x += 4) {
            int incX = 4;
            if (x == limit)
                incX = modulo;

            checkQuad(xa, xb, xc, y10, y21, y02, area, z0, z1, z2, w0, w1, w2, x, y, tpA, tpB, tpC, incX, t);

            xa += y10 << 2,    xb += y21 << 2,    xc += y02 << 2;
        }
        ya += -x10,    yb += -x21,    yc += -x02;
    }
}

