#include "../headers/click_select.h"

#include "../headers/logging.h"

const int clickSelect(const vec4f cs) {
    
    // logVec4f(cs);

    float w = 1 / cs[3];
    vec4f wsc = {
        ((cs[0] / (point_attrib->width >> 1)) - 1.0) * w,
        ((cs[1] / (point_attrib->height >> 1)) - 1.0) * w,
        (1.f / cs[2]) * w,
        w
    };

    wsc = vecxm(wsc, reperspMat);
    wsc[3] = 1.f;
    wsc = vecxm(wsc, *point_mat);

    const int id = clickvsBbox(&tf, &scene, wsc);

    logVec4f(wsc);
    // printf("mesh.id: %d\n", id);
}
const int clickvsBbox(TerrainInfo *tf, Scene *s, vec4f wsc) {
    const int quadIndex = getTerrainQuadIndex(&s->m[Terrain_1], wsc);
    printf("quad_index: %d\n", quadIndex);
    if (quadIndex < 0) {
        fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", quadIndex);
        return -1;
    }

    const int num_of_members = tf->quads[quadIndex].members_indexes;
    for (int i = 0; i < num_of_members; i++) {

        int inner_inx = tf->quads[quadIndex].members[i];

        // if ( s->m[inner_inx].id != s->m[Terrain_1].id ) {

            if ( wsc[2] >= s->m[inner_inx].BB.minZ && wsc[2] < s->m[inner_inx].BB.maxZ ) {

                if ( wsc[0] >= s->m[inner_inx].BB.minX && wsc[0] < s->m[inner_inx].BB.maxX ) {

                    if ( wsc[1] >= s->m[inner_inx].BB.minY && wsc[1] < s->m[inner_inx].BB.maxY ) {
                        printf("mesh.id: %d\n", s->m[inner_inx].id);
                        return s->m[inner_inx].id;
                    }

                }
            }
        // }
    }

    return -1;
}

