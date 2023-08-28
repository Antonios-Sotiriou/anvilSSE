#ifndef _SHADOWMAP_H
#define _SHADOWMAP_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _GENERAL_FUNCTIONS_H
    #include "general_functions.h"
#endif

#ifndef _MATRICES_H
    #include "matrices.h"
#endif

#ifndef _MATH_H
    #include "math.h"
#endif

const void createShadowmap(Mesh m);
const void shadowface(const face f, const Srt srt[]);
const int shadowTest(vec4f frag);

#endif /* _SHADOWMAP_H */

