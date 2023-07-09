#include "headers/grfk_pipeline.h"

extern int HALFH;
extern int HALFW;
extern const float winding(const vec4f a, const vec4f b, const vec4f c);

/* Perspective division. */
const void ppdiv(vec4f *c, const int len) {
    for (int i = 0; i < len; i++) {
            float w = c[i][3];
            c[i] = c[i] / w;
            c[i][3] = w;
    }
}
/* Backface culling.Discarding Triangles that should not be painted.Creating a new dynamic Mesh stucture Triangles array. */
const Mesh bfculling(const Mesh c) {
    Mesh r = c;
    float dpc;
    int counter = 1;
    int index = 0;
    r.f = malloc(sizeof(face));
    if (!r.f)
        fprintf(stderr, "Could not allocate memory - bfculling() - malloc\n");

    for (int i = 0; i < c.f_indexes; i++) {
        dpc = winding(c.v[c.f[i].a[0]], c.v[c.f[i].b[0]], c.v[c.f[i].c[0]]);

        if (dpc < 0.00) {
            r.f = realloc(r.f, sizeof(face) * counter);

            if (!r.f)
                fprintf(stderr, "Could not allocate memory - bfculling() - realloc\n");

            r.f[index] = c.f[i];

            counter++;
            index++;
        }
    }
    r.f_indexes = index;
    free(c.f);
    return r;
}
/* Translates the Mesh's Triangles from world to Screen Coordinates. */
const void viewtoscreen(vec4f *c, const int len) {
    for (int i = 0; i < len; i++) {
        c[i][0] = (++c[i][0]) * HALFW;
        c[i][1] = (++c[i][1]) * HALFH;
        c[i][2] *= 0.5;
        c[i][3] = 1 / c[i][3];
    }
}

