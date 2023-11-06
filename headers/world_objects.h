#ifndef _WORLD_OBJECTS_H
#define _WORLD_OBJECTS_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _MATRICES_H
    #include "matrices.h"
#endif

#ifndef _OBJ_PARSER_H
    #include "obj_parser.h"
#endif

#ifndef _TEST_SHAPES_H
    #include "test_shapes.h"
#endif

/* Defined in general_functions.c. */
extern const float radians(const float value);

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

const void posWorldObjects(Scene *s);
const void loadtexture(Mesh *c);
const Material loadmaterial(const char name[]);
const void createScene(Scene *s);
const void releaseScene(Scene *s);
const void releaseMesh(Mesh *c);
const void initMesh(Mesh *a, const Mesh b);

#endif /* _WORLD_OBJECTS_H */