#ifndef _OBJ_PARSER_H
#define _OBJ_PARSER_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

#ifndef _STDLIB_H
    #include <stdlib.h>
#endif

#ifndef _STRING_H
    #include <string.h>
#endif

const void loadmesh(Mesh *m, const char name[], const unsigned int meshlod);

#endif /* _OBJ_PARSER_H */


