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

#ifndef _COLLISION_DETECTION_H
    #include "collision_detection.h"
#endif

#ifndef _TERRAIN_FUNCTIONS_H
    #include "terrain_functions.h"
#endif

/* Define in main.c. */
extern float DeltaTime;

const void applyGravity(Scene *s);
const void applyForces(Scene *s);

#endif /* _GRAVITY_H */



