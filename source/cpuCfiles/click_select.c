#include "../../headers/components/click_select.h"

const int clickSelect(const vec4f cs) {
    float w = 1 / cs[3];
    vec4f wsc = {
        ((cs[0] / (point_attrib->width >> 1)) - 1.0) * w,
        ((cs[1] / (point_attrib->height >> 1)) - 1.0) * w,
        (1.f / cs[2]) * w,
        w
    };

    wsc = vecxm(wsc, reperspMatrix);
    wsc[3] = 1.f;
    wsc = vecxm(wsc, *point_mat);

    const int id = pointVsBbox(&Gitana, &scene, wsc);

    return id;
}
const int pointVsBbox(TerrainInfo *ti, Scene *s, vec4f wsc) {
    const int quadIndex = getTerrainQuadIndex(&s->m[Terrain_1], wsc);

    if (quadIndex < 0) {
        fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", quadIndex);
        return -1;
    }
    vec4i iwsc = __builtin_convertvector(wsc + 0.5f, vec4i);

    Mesh *cache;
    const int num_of_members = ti->quads[quadIndex].members_indexes;
    for (int i = 0; i < num_of_members; i++) {

        cache = ti->quads[quadIndex].members[i];

        cache->dm = getDimensionsLimits(cache->bbox.v, cache->bbox.v_indexes);
        vec4i min = __builtin_convertvector(cache->dm.min + 0.5, vec4i);
        vec4i max = __builtin_convertvector(cache->dm.max + 0.5f, vec4i);

        if ( iwsc[2] >= min[2] && iwsc[2] <= max[2] ) {

            if ( iwsc[0] >= min[0] && iwsc[0] <= max[0] ) {

                if ( iwsc[1] >= min[1] && iwsc[1] <= max[1] ) {

                    return cache->id;
                }

            }
        }
    }

    logTerrainQuad(*ti, quadIndex);
    return -1;
}

