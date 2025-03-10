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

#ifndef _COORDSYS_ENUM
    #define _COORDSYS_ENUM 1
    typedef enum { P, U, V, N } CoordinatesMembers;
#endif

#ifndef _SCENE_MEMBERS_ENUM
    #define _SCENE_MEMBERS_ENUM 1
    typedef enum { Terrain_1, Player_1, a, b, c, d, Camera_1, Light_1 } SceneMembers;
#endif

/* Primitive struct vec2 with 2 x 32bits floats as members */
typedef float vec2f __attribute__((vector_size(8)));

/* Primitive struct vec4 with 4 x 32bits floats as members */
typedef float vec4f __attribute__((vector_size(16)));

/* Primitive struct vec8 with 8 x 32bits floats as members */
typedef float vec8f __attribute__((vector_size(32)));

/* Primitive struct vec16 with 16 x 32bits floats as members */
typedef float vec16f __attribute__((vector_size(64)));

/* Primitive struct vec with 2 x 32bits ints as members */
typedef int vec2i __attribute__((vector_size(8)));

/* Primitive struct vec4 with 4 x 32bits integers as members */
typedef int vec4i __attribute__((vector_size(16)));

/* Primitive struct vec4 with 4 x 8bits unsigned char as members */
typedef unsigned char vec4c __attribute__((vector_size(4)));

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
    int reflect, texlevels, texlod, init, texture_width, texture_height;
    vec4c *texture;
    char texlvl[9][10];
} Material;

/* Dimensions Limits in any Space. */
typedef struct {
    vec4f min, max;
} DimensionsLimits;

/* Base face aka(triangle) struct. */
typedef struct {
    vec4f v[3];
    vec4f vn[3];
    vec2f vt[3];
} face;

/* Base struct to keep position, orientation of a mesh or of just a point in world. */
typedef struct {
    vec4f v[4];
} Coordinates;

/* Vectors Oria of the Mesh. */
typedef struct {
    vec4f minX, minY, minZ, maxX, maxY, maxZ;
} MeshOria;

/* Bounding box struct. It holds all the information for physics and topology. */
typedef struct {
    face *f;
    vec4f *v;
    int v_indexes, f_indexes;
} Bbox;

/* General Mesh struct from which the scene consists. It holds all the information before the graphic pipeline. */
typedef struct {
    char name[24];
    Material material;
    DimensionsLimits dm;
    Bbox bbox;
    Mat4x4 modelMatrix;
    Coordinates cd;
    vec4f mvdir, velocity;
    Quat Q, r;
    face *f;
    vec8f *vba;
    // void (*drawMesh)(void *args);
    int vba_indexes, f_indexes,                     /* indexes summary. */
    type,                                           /* type of the Mesh. Not sure yet how to categorize them. */
    cull, lodlevels, currentlod, visible,           /* visibillity and lod or texture lod usefull variables. */
    floating, grounded,                             /* variables usefull for physics and kinetics.  */
    quadInit, quadIndex, id,                        /* Terrain quad position and id of meshes index in the scene. */
    tex_index;                                      /* Texture index in the Textures array. */
    float scale, falling_time,                      /* Scale of the mesh and the time that is on Air. */
    momentum,                                       /* The momentum of the mesh. */
    rot_angle,                                      /* The Rotation angle of the mesh. Useful for smooth camera rotation. */
    roll,                                           /* If the mesh is rolling like wheels. */
    mass,                                           /* Mass of the mesh.  m = Weight / 9.81. Newtons */
    weight,                                         /* Weight of the mesh. W = mass * 9.81. Kilogramms */
    fr_coef ,                                       /* Friction of the mesh. Scalar from 0 - 1.  */
    fr_force,                                       /* Friction Force of the mesh. f = f_coeficient * N ( N = normal force in Newtons = Weight ) */
    collision_t;                                    /* Collison time, usefull to sort the collisions. */
} Mesh;

typedef struct {
    int *members;
    unsigned int members_indexes;
} Quad;

typedef struct {
    vec4f pos, normal;
    int quad_index;
} TerrainPointInfo;

/* Struct to hold usefull Terrain information to be available throught the program after we release the height map. */
typedef struct {
    Quad *quads;    /* This pointer to pointer holds the ids of the meshes that belong to each quad index. */
    int vecWidth, vecHeight, quadsArea, quadRows, quadCols;
} TerrainInfo;

/* Scene structs which teams all the meshes into an objects array. */
typedef struct {
    Mesh *m;
    int m_indexes;
    TerrainInfo t;
} Scene;

/* Structs wiche help us to save and pass arround usefull Tile informations for Parallel rendering. */
typedef struct {
    int start_width, start_height, end_width, end_height;
} Tile;

/* Struct to pass to thread functions arguments, that we can use in thread. */
typedef struct {
    Mesh *m;
    Material *mtr;
    Tile *tile;
} Args;

/* Light struct to create different kind of light sources. */
typedef struct {
    vec4f pos, u, v, n, newP;
    Material material;
} Light;

/* This struct here is usefull for sorting array elements without affecting them and also saving their original array indexes. */
typedef struct {
    float y;
    int index;
} Srt;

typedef struct {
    vec4f pos,               /* Screen Space Position. */
    nrm;                     /* The normal of the given position.It is always in View Space */
    Material *mtr;
    vec2f tex;
    int state;
} Fragment;

/* BMP Image file reading structs. */
typedef struct {
    unsigned short Type;
    unsigned int Size;
    unsigned short Res1;
    unsigned short Res2;
    unsigned int OffSet;
} BMP_Header;
typedef struct {
    int RedX;
    int RedY;
    int RedZ;
} Red;
typedef struct {
    int BlueX;
    int BlueY;
    int BlueZ;
} Blue;
typedef struct {
    int GreenX;
    int GreenY;
    int GreenZ;
} Green;
typedef struct {
    Red red_coords;
    Green green_coords;
    Blue blue_coords;
} CS_EndPoints;
typedef struct {
    unsigned int Size;
    signed int Width;
    signed int Height;
    unsigned short Planes;
    unsigned short BitsPerPixel;
    unsigned int compression;
    unsigned int SizeOfBitmap;
    long  HorzResolution;
    long VertResolution;
    unsigned int ColorsUsed;
    unsigned int ColorsImportant;
    unsigned int RedMask;
    unsigned int GreenMask;
    unsigned int BlueMask;
    unsigned int AlphaMask;
    unsigned int CSType;
    CS_EndPoints CSEndPoints;
    unsigned int GammaRed;
    unsigned int GammaGreen;
    unsigned int GammaBlue;
    unsigned int Intent;
    unsigned int ProfileData;
    unsigned int ProfileSize;
	unsigned int Reserved;
} BMP_Info ;
typedef struct {
    BMP_Header header;
    BMP_Info info;
} BMP;

#ifndef _LOGGING_H
    #include "logging.h"
#endif

#endif /* _ANVIL_STRUCTS_H */


