#ifndef _ANVIL_STRUCTS_H
#define _ANVIL_STRUCTS_H 1

#ifndef _XLIB_H
    #include <X11/Xlib.h>
#endif

#ifndef _IMMINTRIN_H
    #include <immintrin.h>
#endif

/* Primitive struct vec4 with 4 x 8bits unsigned char as members */
typedef unsigned char vec4c __attribute__((vector_size(4)));

/* Primitive struct vec4 with 2 x 32bits floats as members */
typedef float vec2f __attribute__((vector_size(8)));

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

/* Material struct to hold the specific for each material values. */
typedef struct {
    char name[25];
    vec4f basecolor;
    vec4f ambient;
    vec4f diffuse;
    vec4f specular;
    float shinniness;
    int reflect, tex_levels;
    signed int texture_height, texture_width;
    vec4c *texture;
    char texlvl[9][10];
} Material;
/* Dimensions Limits in any Space. */
typedef struct {
    float minX, maxX, minY, maxY, minZ, maxZ;
} DimensionsLimits;
/* Base face aka(triangle) struct. */
typedef struct {
    vec4f v[3];
    vec4f vn[3];
    vec2f vt[3];
    vec4f fn;
    int a[3], b[3], c[3];
} face;

/* Initialization matrix */
typedef struct {
    vec4f m[4];
} Mat4x4;

/* Mesh struct which teams all the primitives like faces, vector arrays and textures. */
typedef struct {
    vec4f pivot;
    vec4f *v;
    vec4f *n;
    vec2f *t;
    face *f;
    // void (*drawMesh)(void *args);
    int v_indexes, f_indexes, n_indexes, t_indexes, cull, lvlofdetail;
    Material material;
} Mesh;

/* Scene structs which teams all the meshes into an objects array. */
typedef struct {
    Mesh *m;
    int m_indexes;
} Scene;

/* Light struct to create different kind of light sources. */
typedef struct {
    vec4f pos, u, v, n, newPos;
    Material material;
} Light;

/* This struct here is usefull for sorting array elements without affecting them and also saving their original array indexes. */
typedef struct {
    float y;
    int index;
} Srt;

typedef struct {
    vec4f pos, nrm;
    Material *mtr;
    int state, tex_x, tex_y;
    // void (*drawMesh)(int, int);
} Fragment;

#endif /* _ANVIL_STRUCTS_H */


