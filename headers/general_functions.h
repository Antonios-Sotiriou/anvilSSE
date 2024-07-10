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

#ifndef _DRAW_FUNCTIONS_H
    #include "draw_functions.h"
#endif

/* Define in main.c */
extern Mat4x4 lookAt, worldMat, viewMat, perspMat, orthoMat;
extern float FPlane, NPlane;
extern int HALFW, HALFH, PROJECTIONVIEW;
extern Tile *tiles;

const void swap(void *a, void *b, unsigned long size);
const float radians(const float value);
const DimensionsLimits getDimensionsLimits(vec4f va[], const int len);
const void loadMaterial(Material *mtr, const char name[]);
const void loadtexture(Mesh *m, const unsigned int lvl);
const void readHeightmap(const char path[]);
const void adoptdetailMesh(Mesh *m, const int dist);
const void adoptdetailTexture(Mesh *m, const int dist);
const void enWorldMesh(Mesh *m);
const void enworldBbox(Mesh *m);
const void placeMesh(Mesh *m, const vec4f pos);
const int frustumCulling(vec4f v[], const int v_indexes, const int thread_id);
const void checkVisibles(Scene *s, Mesh *m, const int viewProj);
const void displayVec4f(const vec4f v_start, const vec4f v_end, const Mat4x4 vm);
const void displayBbox(vec4f v[], const int v_indexes, const Mat4x4 vm);
const void initMesh(Mesh *a, const Mesh *b);
const void releaseMesh(Mesh *m);

#endif /* _GENERAL_FUNCTIONS_H */


