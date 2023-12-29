#ifndef _GENERAL_FUNCTIONS_H
#define _GENERAL_FUNCTIONS_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _MATRICES_H
    #include "matrices.h"
#endif

#ifndef _OBJ_PARSER_H
    #include "obj_parser.h"
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

#ifndef _STDLIB_H
    #include <stdlib.h>
#endif

#ifndef _STRING_H
    #include <string.h>
#endif

typedef struct {
    unsigned short Type;
    unsigned int Size;
    unsigned short Res1;
    unsigned short Res2;
    unsigned int OffSet;
} BMP_Header;
/* The structs in BMP_Info header which define the endpoints */
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
} Color_Palete;
/* End of Endpoints structs */
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
    // unsigned int ColorsUsed;
    // unsigned int ColorsImportant;
    // unsigned int RedMask;
    // unsigned int GreenMask;
    // unsigned int BlueMask;
    // unsigned int AlphaMask;
    // unsigned int CSType;
    // Color_Palete EndPoints;
    // unsigned int GammaRed;
    // unsigned int GammaGreen;
    // unsigned int GammaBlue;
    // unsigned int Intent;
    // unsigned int ProfileData;
    // unsigned int ProfileSize;
	unsigned int Reserved;
} BMP_Info ;

#ifndef _DRAW_FUNCTIONS_H
    #include "draw_functions.h"
#endif

/* Define in main.c */
extern Mat4x4 lookAt, worldMat;
extern float FPlane, NPlane;
extern int HALFW, HALFH;

extern Window mainwin;
extern Display *displ;
extern GC gc;

const void swap(void *a, void *b, unsigned long size);
const float radians(const float value);
const DimensionsLimits getDimensionsLimits(vec4f va[], const int len);
const void loadmaterial(Material *mtr, const char name[]);
const void loadtexture(Mesh *m, const unsigned int lvl);
const void adoptdetailMesh(Mesh *m);
const void adoptdetailTexture(Mesh *m);
const void reWorldMesh(Mesh *m);
const void placeMesh(Mesh *m, const vec4f pos);
const void frustumCulling(Mesh *m, const int len);
const void initMesh(Mesh *a, const Mesh *b);
const void releaseMesh(Mesh *m);

#endif /* _GENERAL_FUNCTIONS_H */


