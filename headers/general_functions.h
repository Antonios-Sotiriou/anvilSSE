#ifndef _GENERAL_FUNCTIONS_H
#define _GENERAL_FUNCTIONS_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _QUATERNIONS_H
    #include "quaternions.h"
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

#ifndef _DRAW_FUNCTIONS_H
    #include "draw_functions.h"
#endif

/* Define in main.c */
extern Mat4x4 lookAt, worldMat, perspMat, orthoMat;
extern float FPlane, NPlane;
extern int HALFW, HALFH, PROJECTIONVIEW;

extern Window mainwin;
extern Display *displ;
extern GC gc;

const void swap(void *a, void *b, unsigned long size);
const float radians(const float value);
const DimensionsLimits getDimensionsLimits(vec4f va[], const int len);
const void loadmaterial(Material *mtr, const char name[]);
const void loadtexture(Mesh *m, const unsigned int lvl);
const void readHeightmap(const char path[]);
const void adoptdetailMesh(Mesh *m);
const void adoptdetailTexture(Mesh *m);
const void reWorldMesh(Mesh *m);
const void placeMesh(Mesh *m, const vec4f pos);
const void frustumCulling(Mesh *m, const int len);
const int checkVisibles(Scene *s, Mesh *m);
const void initMesh(Mesh *a, const Mesh *b);
const void releaseMesh(Mesh *m);

#endif /* _GENERAL_FUNCTIONS_H */


