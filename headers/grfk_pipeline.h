#ifndef _GRFK_PIPELINE_H
#define _GRFK_PIPELINE_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

const void ppdiv(vec4f *c, const int len);
const Mesh bfculling(const Mesh c);
const void viewtoscreen(vec4f *c, const int len);

#endif /* _GRFK_PIPELINE_H */