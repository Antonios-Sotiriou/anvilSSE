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

/* Defined in main.c. */
extern Light sunlight;

/* Defined in world_objects.c. */
extern const void releaseMesh(Mesh *c);

/* Defined in general_functions.c. */
extern const float radians(const float value);

/* Objects Rotation functions */
const void rotate_x(Mesh *c, const float angle);
const void rotate_y(Mesh *c, const float angle);
const void rotate_z(Mesh *c, const float angle);
const void rotate_origin(Mesh *obj, const float angle, float x, float y, float z);
const void rotate_light(Light *l, const vec4f pivot, const float angle, float x, float y, float z);
const void rotate_light_cam(Mesh *l, const vec4f pivot, const float angle, float x, float y, float z);

#endif /* _KINETICS_H */


