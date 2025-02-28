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

#ifndef _MESH_LOADER_H
    #include "mesh_loader.h"
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

/* Define in main.c */
extern Display *displ;
extern Window mainwin;
extern GC gc;
extern vec4i half_screen;
extern Mat4x4 lookAt, worldMatrix, viewMatrix, perspMatrix, orthoMatrix;
extern float FPlane, NPlane;
extern int PROJECTIONVIEW;
extern Tile *tiles;

const void swap(void *a, void *b, unsigned long size);
const float radians(const float value);
const DimensionsLimits getDimensionsLimits(vec4f va[], const int len);
const  MeshOria getMeshOria(vec4f va[], const int len);
void roundMesh(Mesh *m);
const void loadMaterial(Material *mtr, const char name[]);
const void loadtexture(Mesh *m, const unsigned int lvl);
const void adoptdetailMesh(Mesh *m, const int dist);
const void adoptdetailTexture(Mesh *m, const int dist);
const void enworldBbox(Mesh *m);
const void checkVisibles(Scene *s, Mesh *m, const int viewProj);
const void displayPoint(const vec4f p, const Mat4x4 vm, const int color);
const void displayMeshKinetics(Mesh *m, const Mat4x4 vm);
const void drawLine(vec4f start, vec4f end, const Mat4x4 vm);
const void displayFace(face *f, const Mat4x4 vm);
const void displayFilledFace(face *f, const Mat4x4 vm);
const void displayBbox(Mesh *m, const Mat4x4 vm);
const void displayBboxFaces(Mesh *m, const Mat4x4 vm);
const void displayfacedm(face f, const Mat4x4 vm);
const void initMesh(Mesh *a, const Mesh *b);
const void releaseMesh(Mesh *m);

#endif /* _GENERAL_FUNCTIONS_H */


