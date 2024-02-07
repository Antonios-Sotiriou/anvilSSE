#ifndef _SCENE_OBJECTS_H
#define _SCENE_OBJECTS_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _QUATERNIONS_H
    #include "quaternions.h"
#endif

#ifndef _GENERAL_FUNCTIONS_H
    #include "general_functions.h"
#endif

#ifndef _TEST_SHAPES_H
    #include "test_shapes.h"
#endif

#ifndef _TERRAIN_FUNCTIONS_H
    #include "terrain_functions.h"
#endif

const void initWorldObjects(Scene *s);
const void createScene(Scene *s);
const void releaseScene(Scene *s);

#endif /* _SCENE_OBJECTS_H */