#ifndef _GRAVITY_H
#define _GRAVITY_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _MATRICES_H
    #include "matrices.h"
#endif

#ifndef _STRING_H
    #include <string.h>
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

const void applyGravity(Scene *s, const float FallTime);

#endif /* _GRAVITY_H */



