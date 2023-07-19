#ifndef _KINETICS_H
#define _KINETICS_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _MATRICES_H
    #include "matrices.h"
#endif

#ifndef _QUATERNIONS_H
    #include "quaternions.h"
#endif

// #ifndef _GENERAL_FUNCTIONS_H
//     #include "general_functions.h"
// #endif

/* Objects Rotation functions */
const void rotate_x(Mesh *c, const float angle);
const void rotate_y(Mesh *c, const float angle);
const void rotate_z(Mesh *c, const float angle);
const void rotate_origin(Mesh *obj, const float angle, float x, float y, float z);
const void rotate_light(Light *l, const float angle, float x, float y, float z);

#endif /* _KINETICS_H */


