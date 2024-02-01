#ifndef _LOGGING_H
#define _LOGGING_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _QUATERNIONS_H
    #include "quaternions.h"
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

const void logEvent(const XEvent e);
const void logVec4f(const vec4f v);
const void logVec4i(const vec4i v);
const void logVec4c(const vec4c v);
const void logVec2f(const vec2f v);
const void logMesh(const Mesh m);
const void logFace(const face f, const int vec, const int tex, const int norm);
const void logMatrix(const Mat4x4 m);
const void logQuat(const Quat q);
const void logDm(const DimensionsLimits dm);

#endif /* _LOGGING_H */


