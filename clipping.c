#include "headers/clipping.h"

extern const void swap(void *a, void *b, unsigned long size);

/* Normal Grafik Pipeline clipping algorythm. */
const Mesh clipp(Mesh *c, vec4f plane_p, vec4f plane_n) {

    Mesh r = *c;
    size_t face_size = sizeof(face);
    r.f = malloc(face_size);
    int index = 0;
    int dynamic_inc = 1;

    int clipped_count = 0;
    face clipped[2];
    for (int i = 0; i < c->f_indexes; i++) {

        clipped_count = clipp_triangle(plane_p, plane_n, c->f[i], &clipped[0], &clipped[1]);

        if (clipped_count) {

            if (clipped_count == 1) {
                r.f = realloc(r.f, face_size * dynamic_inc);
                r.f[index] = clipped[0];
                index++;
                dynamic_inc++;
            } else if (clipped_count == 2) {
                r.f = realloc(r.f, face_size * (dynamic_inc + 1));
                r.f[index] = clipped[0];

                r.f[index + 1] = clipped[1];
                index += 2;
                dynamic_inc += 2;
            } else if (clipped_count == 3) {
                r.f = realloc(r.f, face_size * dynamic_inc);
                r.f[index] = c->f[i];
                index++;
                dynamic_inc++;
            }
        }
    }
    r.f_indexes = index;
    free(c->f);
    return r;
}
const vec4f plane_intersect(vec4f plane_p, vec4f plane_n, vec4f line_start, vec4f line_end, float *t) {
    float plane_d = -dot_product(plane_n, plane_p);
    float ad = dot_product(line_start, plane_n);
    float bd = dot_product(line_end, plane_n);
    *t = ((-plane_d - ad) / (bd - ad));
    vec4f line_ste = line_end - line_start;
    vec4f line_ti = line_ste * (*t);

    return line_start + line_ti;
}
/* Return signed shortest distance from point to plane, plane normal must be normalised. */
const float dist(vec4f plane_p, vec4f plane_n, vec4f v) {
    vec4f r = plane_n * v;
    return ( (r[0] + r[1] + r[2]) - dot_product(plane_n, plane_p) );
}
const int clipp_triangle(vec4f plane_p, vec4f plane_n, face in_t, face *out_t1, face *out_t2) {
    int inside_points[3];     int inside_count = 0;
    int outside_points[3];    int outside_count = 0;

    // Get signed distance of each point in triangle to plane.
    float d0 = dist(plane_p, plane_n, in_t.v[0]);
    float d1 = dist(plane_p, plane_n, in_t.v[1]);
    float d2 = dist(plane_p, plane_n, in_t.v[2]);

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
        return 0; /* face is outside and must be ignored. */
    } else if (inside_count == 3) {
        return 3; /* face is inside and it needs no clipping. */
    } else if (inside_count == 1 && outside_count == 2) {
        out_t1->v[0] = in_t.v[inside_points[0]];
        out_t1->vt[0] = in_t.vt[inside_points[0]];
        out_t1->vn[0] = in_t.vn[inside_points[0]];

        if ( inside_points[0] != 1 )
            swap(&outside_points[0], &outside_points[1], 4);

        out_t1->v[1] = plane_intersect(plane_p, plane_n, in_t.v[inside_points[0]], in_t.v[outside_points[1]], &t);
        out_t1->vt[1] = in_t.vt[inside_points[0]] + (t * (in_t.vt[outside_points[1]] - in_t.vt[inside_points[0]]));
        out_t1->vn[1] = in_t.vn[inside_points[0]] + (t * (in_t.vn[outside_points[1]] - in_t.vn[inside_points[0]]));

        out_t1->v[2] = plane_intersect(plane_p, plane_n, in_t.v[inside_points[0]], in_t.v[outside_points[0]], &t);
        out_t1->vt[2] = in_t.vt[inside_points[0]] + (t * (in_t.vt[outside_points[0]] - in_t.vt[inside_points[0]]));
        out_t1->vn[2] = in_t.vn[inside_points[0]] + (t * (in_t.vn[outside_points[0]] - in_t.vn[inside_points[0]]));
        return 1; /* A new face is created. */
    } else if (inside_count == 2 && outside_count == 1) {
        if ( outside_points[0] != 1 )
            swap(&inside_points[0], &inside_points[1], 4);

        out_t1->v[0] = in_t.v[inside_points[0]];
        out_t1->vt[0] = in_t.vt[inside_points[0]];
        out_t1->vn[0] = in_t.vn[inside_points[0]];

        out_t1->v[1] = plane_intersect(plane_p, plane_n, in_t.v[inside_points[0]], in_t.v[outside_points[0]], &t);
        out_t1->vt[1] = in_t.vt[inside_points[0]] + (t * (in_t.vt[outside_points[0]] - in_t.vt[inside_points[0]]));
        out_t1->vn[1] = in_t.vn[inside_points[0]] + (t * (in_t.vn[outside_points[0]] - in_t.vn[inside_points[0]]));

        out_t1->v[2] = in_t.v[inside_points[1]];
        out_t1->vt[2] = in_t.vt[inside_points[1]];
        out_t1->vn[2] = in_t.vn[inside_points[1]];
        /* ------------------------------------------------------------------------------------ */
        out_t2->v[0] = out_t1->v[1];
        out_t2->vt[0] = out_t1->vt[1];
        out_t2->vn[0] = out_t1->vn[1];

        out_t2->v[1] = plane_intersect(plane_p, plane_n, in_t.v[inside_points[1]], in_t.v[outside_points[0]], &t);
        out_t2->vt[1] = in_t.vt[inside_points[1]] + (t * (in_t.vt[outside_points[0]] - in_t.vt[inside_points[1]]));
        out_t2->vn[1] = in_t.vn[inside_points[1]] + (t * (in_t.vn[outside_points[0]] - in_t.vn[inside_points[1]]));

        out_t2->v[2] = in_t.v[inside_points[1]];
        out_t2->vt[2] = in_t.vt[inside_points[1]];
        out_t2->vn[2] = in_t.vn[inside_points[1]];
        return 2; /* Two new faces are created. */
    }
    return 0;
}


/* Shadow Pipeline clipping algorythm. */
const Mesh shadowclipp(const Mesh c, vec4f plane_p, vec4f plane_n) {

    Mesh r = c;
    size_t face_size = sizeof(face);
    r.f = malloc(face_size);
    int index = 0;
    int dynamic_inc = 1;

    int clipped_count = 0;
    face clipped[2];
    for (int i = 0; i < c.f_indexes; i++) {

        clipped_count = shadowclipp_triangle(plane_p, plane_n, c.f[i], &clipped[0], &clipped[1]);

        if (clipped_count) {

            if (clipped_count == 1) {
                r.f = realloc(r.f, face_size * dynamic_inc);
                r.f[index] = clipped[0];
                index++;
                dynamic_inc++;
            } else if (clipped_count == 2) {
                r.f = realloc(r.f, face_size * (dynamic_inc + 1));
                r.f[index] = clipped[0];

                r.f[index + 1] = clipped[1];
                index += 2;
                dynamic_inc += 2;
            } else if (clipped_count == 3) {
                r.f = realloc(r.f, face_size * dynamic_inc);
                r.f[index] = c.f[i];
                index++;
                dynamic_inc++;
            }
        }
    }
    r.f_indexes = index;
    free(c.f);
    return r;
}
const int shadowclipp_triangle(vec4f plane_p, vec4f plane_n, face in_t, face *out_t1, face *out_t2) {
    int inside_points[3];     int inside_count = 0;
    int outside_points[3];    int outside_count = 0;

    // Get signed distance of each point in triangle to plane.
    float d0 = dist(plane_p, plane_n, in_t.v[0]);
    float d1 = dist(plane_p, plane_n, in_t.v[1]);
    float d2 = dist(plane_p, plane_n, in_t.v[2]);

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
        return 0; /* face is outside and must be ignored. */
    } else if (inside_count == 3) {
        return 3; /* face is inside and it needs no clipping. */
    } else if (inside_count == 1 && outside_count == 2) {
        out_t1->v[0] = in_t.v[inside_points[0]];

        if ( inside_points[0] != 1 )
            swap(&outside_points[0], &outside_points[1], 4);

        out_t1->v[1] = plane_intersect(plane_p, plane_n, in_t.v[inside_points[0]], in_t.v[outside_points[1]], &t);

        out_t1->v[2] = plane_intersect(plane_p, plane_n, in_t.v[inside_points[0]], in_t.v[outside_points[0]], &t);
        return 1; /* A new face is created. */
    } else if (inside_count == 2 && outside_count == 1) {
        if ( outside_points[0] != 1 )
            swap(&inside_points[0], &inside_points[1], 4);

        out_t1->v[0] = in_t.v[inside_points[0]];

        out_t1->v[1] = plane_intersect(plane_p, plane_n, in_t.v[inside_points[0]], in_t.v[outside_points[0]], &t);

        out_t1->v[2] = in_t.v[inside_points[1]];
        /* ------------------------------------------------------------------------------------ */
        out_t2->v[0] = out_t1->v[1];

        out_t2->v[1] = plane_intersect(plane_p, plane_n, in_t.v[inside_points[1]], in_t.v[outside_points[0]], &t);

        out_t2->v[2] = in_t.v[inside_points[1]];
        return 2; /* Two new faces are created. */
    }
    return 0;
}


