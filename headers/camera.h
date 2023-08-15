#ifndef _CAMERA_H
#define _CAMERA_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _QUATERNIONS_H
    #include "quaternions.h"
#endif

#ifndef _VECTORS_MATH_H
    #include "vectors_math.h"
#endif

const void look_left(vec4f *g, const float angle);
const void look_right(vec4f *g, const float angle);
const void look_up(vec4f *g, const float angle);
const void look_down(vec4f *g, const float angle);
const void move_forward(vec4f *g, const float vel);
const void move_backward(vec4f *g, const float vel);
const void move_left(vec4f *g, const float vel);
const void move_right(vec4f *g, const float vel);
const void move_up(vec4f *g, const float vel);
const void move_down(vec4f *g, const float vel);

#endif /* _CAMERA_H */