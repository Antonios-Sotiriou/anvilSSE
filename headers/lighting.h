#ifndef _LIGHTING_H
#define _LIGHTING_H 1

#ifndef _STRING_H
    #include <string.h>
#endif

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _MATRICES_H
    #include "matrices.h"
#endif


#ifndef _VECTORS_MATH_H
    #include "vectors_math.h"
#endif

const float phong(vec4f nm, const Material mtr, const float pixX, const float pixY, const float pixZ, const float pixW);

#endif /* _LIGHTING_H */


