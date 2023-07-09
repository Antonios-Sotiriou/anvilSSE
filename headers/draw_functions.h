#ifndef _DRAW_FUNCTIONS_H
#define _DRAW_FUNCTIONS_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

const void drawLine(float x1, float y1, float x2, float y2, const float red, const float green, const float blue);

const void fillTriangle(void);
const void fillGeneral(const int minX, const int maxX, const int minY, const int maxY);

#endif /* _DRAW_FUNCTIONS_H */

