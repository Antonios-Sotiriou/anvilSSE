#include "headers/height_pipeline.h"

const static int sortVertices(face *f, vec4f *pivot);
const static int checkIfInside(face *f, vec4i pv);
const static vec4i hmask = { 2, 0, 1, 3 };
#include "headers/logging.h"
const void heightPipeline(Scene *s, vec4f *pivot) {
    face temp;
    system("clear\n");
    for (int i = 0; i < s->m_indexes; i++) {
        if (s->m[i].type == Terrain) {
            for (int j = 0; j < s->m[i].f_indexes; j += 9) {

                temp.v[0] = s->m[i].v[s->m[i].f[j]];
                temp.v[1] = s->m[i].v[s->m[i].f[j + 3]];
                temp.v[2] = s->m[i].v[s->m[i].f[j + 6]];

                const float height = sortVertices(&temp, pivot);
                // pivot[1] += height;
            }
        }
    }
}
const static int sortVertices(face *f, vec4f *pivot) {
    /* Creating 2Arrays for X and Y values to sort them-> */
    for (int i = 0; i < 3; i++) {
        f->v[i][0] = ((1.f + f->v[i][0]) * 50.f) + 0.5;
        f->v[i][2] = ((1.f + f->v[i][2]) * 50.f) + 0.5;
    }

    vec4i pv = __builtin_convertvector(*pivot + 0.5f, vec4i);
    pv[0] = (1.f + pv[0]) * 50.f;
    pv[2] = (1.f + pv[2]) * 50.f;

    return checkIfInside(f, pv);
}
const static int checkIfInside(face *f, vec4i pv) {
    const vec4i xs = { f->v[0][0],  f->v[1][0], f->v[2][0], 0};
    const vec4i zs = { f->v[0][2],  f->v[1][2], f->v[2][2], 0};

    const vec4i xmx = xs - __builtin_shuffle(xs, hmask);
    const vec4i zmz = zs - __builtin_shuffle(zs, hmask);

    const int tps0 = ((zmz[0] == 0) && (zs[1] < zs[2])) || (zmz[0] < 0) ? 1 : 0;
    const int tps1 = ((zmz[1] == 0) && (zs[2] < zs[0])) || (zmz[1] < 0) ? 1 : 0;
    const int tps2 = ((zmz[2] == 0) && (zs[0] < zs[1])) || (zmz[2] < 0) ? 1 : 0;

    // const float area = ((xs[0] - xs[2]) * zmz[1]) - ((zs[0] - zs[2]) * xmx[1]);
    const float area = ((xs[0] - xs[2]) * zmz[1]) - ((zs[0] - zs[2]) * xmx[1]);
    vec4i za = ((pv[0] - xs) * zmz) - ((pv[2] - zs) * xmx);
    // printf("Inside: area: %f\n", area);
    // logVec4i(za);
    // printf("tsp0: %d, tps1: %d, tps2: %d\n", tps0, tps1, tps2);

    const int xa0 = ( (tps0) && (!za[0]) ) ? -1 : za[0];
    const int xa1 = ( (tps1) && (!za[1]) ) ? -1 : za[1];
    const int xa2 = ( (tps2) && (!za[2]) ) ? -1 : za[2];

    if ( (xa0 | xa1 | xa2) > 0 ) {
        vec4f a = __builtin_convertvector(za, vec4f) / area;

        const vec4f height = a[0] * f->v[2] + a[1] * f->v[0] + a[2] * f->v[1];
        printf("Inside: height = %f\n", height[1]);
        // logVec4f(a); 
        return height[1];
    }
    // printf("Outside\n");
    return 0;
}



