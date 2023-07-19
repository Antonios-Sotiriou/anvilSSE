#ifndef _QUATERNIONS_H
#define _QUATERNIONS_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

#ifndef _MATH_H
    #include <math.h>
#endif

typedef struct {
    float w;
    vec4f v;
} Quat;

Quat unitQuat(void);
Quat setQuat(const float w, const vec4f vec);
void normalizeQuat(Quat *q);
Quat conjugateQuat(const Quat q);
float magnitudeQuat(const Quat q);
Quat rotationQuat(const float angle, const vec4f axis);
Quat eulertoQuat(const float roll, const float yaw, const float pitch);
Quat multiplyQuats(const Quat q1, const Quat q2);
Quat addQuats(const Quat q1, const Quat q2);
Mat4x4 MatfromQuat(const Quat q, const vec4f v);

#endif /* _QUATERNIONS_H */


