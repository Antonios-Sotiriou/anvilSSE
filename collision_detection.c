#include "headers/collision_detection.h"

const void objectTerrainCollision(Mesh *terrain, Mesh *object) {
    const float height = getTerrainHeight(terrain, object->pivot, object);

    float height_diff = height - (object->pivot[1] - object->scale);
    if (height_diff >= 0) {
        object->grounded = 1;
        object->falling_time = 0;
        object->rahm = 0;        /* Reset the rahm here but we probably gonna handle it somewhere else. */
    }
    if (object->grounded) {
        object->pivot[1] += height_diff;
    }
}



