#ifndef _HEIGHT_MAP_H
#define _HEIGHT_MAP_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _GENERAL_FUNCTIONS_H
    #include "general_functions.h"
#endif

#ifndef _MATRICES_H
    #include "matrices.h"
#endif

#ifndef _CLIPPING_H
    #include "clipping.h"
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

const void heightPipeline(Scene *s, vec4f *pivot);

#endif /* _HEIGHT_MAP_H */



