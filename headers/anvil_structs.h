#ifndef _ANVIL_STRUCTS_H
#define _ANVIL_STRUCTS_H 1

#ifndef _XLIB_H
    #include <X11/Xlib.h>
#endif

#ifndef _IMMINTRIN_H
    #include <immintrin.h>
#endif

#ifndef _ANVIL_FLAGS_H
    #include "anvil_flags.h"
#endif

/* Primitive struct vec4 with 4 x 8bits unsigned char as members */
typedef unsigned char vec4c __attribute__((vector_size(4)));

/* Primitive struct vec with 2 x 32bits floats as members */
typedef float vec2f __attribute__((vector_size(8)));

/* Primitive struct vec4 with 4 x 32bits integers as members */
typedef int vec4i __attribute__((vector_size(16)));

/* Primitive struct vec4 with 4 x 32bits floats as members */
typedef float vec4f __attribute__((vector_size(16)));

/* intrinsics float union. */
typedef union {
    __m128 mm;
    vec4f f;
} v128f;

/* intrinsics integer union. */
typedef union {
    __m128i mm;
    vec4i i;
} v128i;

/* Initialization matrix */
typedef struct {
    vec4f m[4];
} Mat4x4;

typedef struct {
    float w;
    vec4f v;
} Quat;

/* Material struct to hold the specific for each material values. */
typedef struct {
    char name[24];
    vec4f basecolor;
    vec4f ambient;
    vec4f diffuse;
    vec4f specular;
    float shinniness;
    int reflect, texlevels, texlod;
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
} face;

/* General Mesh struct from which the scene consists. It holds all the information before the graphic pipeline. */
typedef struct {
    char name[24];
    vec4f pivot, rotation;
    vec4f *v;
    vec4f *n;
    vec2f *t;
    unsigned int *f;
    // void (*drawMesh)(void *args);
    int v_indexes, n_indexes, t_indexes, f_indexes, cull, lodlevels, meshlod, visible, type, floating, relaxing, grounded;
    float scale;
    Quat Q;
    Material material;
} Mesh;

// /* Inside graphic pipeline struct. Holds necessery data for the 1st step of the graphic pipeline. */
typedef struct {
    vec4f *v;
    vec4f *n;
    vec2f *t;
    int v_indexes, n_indexes, t_indexes, cull;
} MeshStepOne;

/* Inside graphic pipeline struct. Holds necessery data for the 2nd step of the graphic pipeline. */
typedef struct {
    face *f;
    int f_indexes, cull;
} MeshStepTwo;

/* Base Height face aka(triangle) struct. */
typedef struct {
    vec4f v[3];
    vec4f vn[3];
} Heightface;

// /* Inside height pipeline struct. Holds necessery data for the 1st step of the height pipeline. */
typedef struct {
    vec4f *v;
    vec4f *n;
    int v_indexes, n_indexes, cull;
} MeshHeightStepOne;

/* Inside height pipeline struct. Holds necessery data for the 2nd step of the height pipeline. */
typedef struct {
    Heightface *f;
    int f_indexes, cull;
} MeshHeightStepTwo;

/* Base Shadow face aka(triangle) struct. */
typedef struct {
    vec4f v[3];
} Shadowface;

// /* Inside graphic pipeline struct. Holds necessery data for the 1st step of the graphic pipeline. */
typedef struct {
    vec4f *v;
    int v_indexes, cull;
} MeshShadowStepOne;

/* Inside graphic pipeline struct. Holds necessery data for the 2nd step of the graphic pipeline. */
typedef struct {
    Shadowface *f;
    int f_indexes, cull;
} MeshShadowStepTwo;

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


