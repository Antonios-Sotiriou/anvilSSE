#ifndef _CAMERA_H
#define _CAMERA_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _QUATERNIONS_H
    #include "quaternions.h"
#endif

#ifndef _MATRICES_H
    #include "matrices.h"
#endif

#ifndef _VECTORS_MATH_H
    #include "vectors_math.h"
#endif

/* Defined in main.c. */
extern float DeltaTime;

const void look_left(Mesh *m, const float angle);
const void look_right(Mesh *m, const float angle);
const void look_up(Mesh *m, const float angle);
const void look_down(Mesh *m, const float angle);
const void move_forward(Mesh *m, const float force);
const void move_backward(Mesh *m, const float force);
const void move_left(Mesh *m, const float force);
const void move_right(Mesh *m, const float force);
const void move_up(Mesh *m, const float force);
const void move_down(Mesh *m, const float force);

#endif /* _CAMERA_H */