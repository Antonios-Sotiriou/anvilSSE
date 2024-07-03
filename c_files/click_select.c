#include "../headers/click_select.h"

#include "../headers/logging.h"
#include "../headers/terrain_functions.h"

const int clickSelect(const vec4f cs) {
    
    logVec4f(cs);

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

    // printf("mesh.id: %d\n", id);
    // logVec4i(__builtin_convertvector(wsc + 0.5f, vec4i));
    // logVec4f(wsc);

    return id;
}
const int clickvsBbox(TerrainInfo *tf, Scene *s, vec4f wsc) {
    const int quadIndex = getTerrainQuadIndex(&s->m[Terrain_1], wsc);
    vec4i iwsc = __builtin_convertvector(wsc + 0.5f, vec4i);

    // printf("quad_index: %d\n", quadIndex);
    // printQuad(quadIndex);
    if (quadIndex < 0) {
        fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", quadIndex);
        return -1;
    }

    const int num_of_members = tf->quads[quadIndex].members_indexes;
    for (int i = 0; i < num_of_members; i++) {

        int inner_inx = tf->quads[quadIndex].members[i];

        if ( s->m[inner_inx].id != s->m[Terrain_1].id ) {

            s->m[inner_inx].dm = getDimensionsLimits(s->m[inner_inx].bbox.v, s->m[inner_inx].bbox.v_indexes);
            vec4i min = __builtin_convertvector(s->m[inner_inx].dm.min + 0.5, vec4i);
            vec4i max = __builtin_convertvector(s->m[inner_inx].dm.max + 0.5f, vec4i);

            if ( iwsc[2] >= min[2] && iwsc[2] <= max[2] ) {

                if ( iwsc[0] >= min[0] && iwsc[0] <= max[0] ) {

                    if ( iwsc[1] >= min[1] && iwsc[1] <= max[1] ) {
                        // logDm(s->m[i].dm);
                        // logVec4i(min);
                        // logVec4i(max);
                        return s->m[inner_inx].id;
                    }

                }
            }
        }
    }

    // const int num_of_members = s->m_indexes;
    // for (int i = 0; i < num_of_members; i++) {

    //     // int inner_inx = tf->quads[quadIndex].members[i];
    //     // s->m[i].bbox.v = setvecsarrayxm(s->m[i].bbox.v, s->m[i].bbox.v_indexes, *point_mat);
    //     // s->m[i].dm = getDimensionsLimits(s->m[i].bbox.v, s->m[i].bbox.v_indexes);

    //     if ( s->m[i].id != s->m[Terrain_1].id ) {

    //         s->m[i].dm = getDimensionsLimits(s->m[i].bbox.v, s->m[i].bbox.v_indexes);
    //         vec4i min = __builtin_convertvector(s->m[i].dm.min + 0.5, vec4i);
    //         vec4i max = __builtin_convertvector(s->m[i].dm.max + 0.5f, vec4i);

    //         if ( iwsc[2] >= min[2] && iwsc[2] <= max[2] ) {

    //             if ( iwsc[0] >= min[0] && iwsc[0] <= max[0] ) {

    //                 if ( iwsc[1] >= min[1] && iwsc[1] <= max[1] ) {
    //                     logDm(s->m[i].dm);
    //                     logVec4i(min);
    //                     logVec4i(max);
    //                     return s->m[i].id;
    //                 }

    //             }
    //         }
    //     }
    // }

    return -1;
}

