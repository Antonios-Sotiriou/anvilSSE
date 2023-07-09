#ifndef _ANVIL_STRUCTS_H
#define _ANVIL_STRUCTS_H 1

#ifndef _XLIB_H
    #include <X11/Xlib.h>
#endif

#ifndef _IMMINTRIN_H
    #include <immintrin.h>
#endif

/* Primitive struct vec4 with 4 x 32bits integers as members */
typedef int vec4i __attribute__((vector_size(16)));
typedef union {
    __m128i mm;
    vec4i i;
} v128i;

/* Primitive struct vec4 with 4 x 32bits floats as members */
typedef float vec4f __attribute__((vector_size(16)));
typedef union {
    __m128 mm;
    vec4f f;
} v128f;

typedef struct {
    int a[3], b[3], c[3];
} face;

/* Initialization matrix */
typedef struct {
    vec4f m[4];
} Mat4x4;

/* Mesh struct which teams all the primitives like faces, vector arrays and textures. */
typedef struct {
    char texture_file[50];
    vec4f *v;
    face *f;
    signed int texture_height;
    signed int texture_width;
    int v_indexes;
    int f_indexes;
} Mesh;

/* Scene structs which teams all the meshes into an objects array. */
typedef struct {
    Mesh *m;
    int m_indexes;
} Scene;

#endif /* _ANVIL_STRUCTS_H */

