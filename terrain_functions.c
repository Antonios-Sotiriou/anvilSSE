#include "headers/terrain_functions.h"

const static vec4i hmask = { 1, 2, 0, 3 };

/* Retrieves Terrain *t height at given coords and, sets given meshes *m terain quadIndex to the id of the quad at those coords. */
const float getTerrainHeight(Mesh *t, vec4f coords, Mesh *m) {
    float quad_len = t->scale / tf.vecWidth;
    const int t_limit = t->scale - quad_len;

    vec4f t_coords = coords - (t->pivot - (t->scale / 2.f));
    if ( (t_coords[0] >= t_limit || t_coords[0] < 0) || (t_coords[2] >= t_limit || t_coords[2] < 0) ) {
        fprintf(stderr, "Out of terrain Limits -- getTerrainHeight.\n");
        return 0;
    } 

    /* Function to get t quads indexes. */
    vec4i pos = __builtin_convertvector((t_coords / (t->scale)) * (float)tf.vecWidth, vec4i);
    const int quad_index = (pos[2] * tf.quadRows) + pos[0];
    /* Set meshes m quadIndex to index. */
    m->quadIndex = quad_index;
    // printf("Quad index: %d\n", quad_index);
    /* Every quad has two faces incrementally. Every face constists of 9 indexes for vectors, normals, textors.
        So to get the right index we multiply faces with 9, because indexes are stored raw until now. */
    const int Upperface = (quad_index * 2) * 9;
    const int Lowerface = ((quad_index * 2) + 1) * 9;

    /* FInd in which triangle we are. */
    float x = ((t_coords[0] / quad_len));
    float z = ((t_coords[2] / quad_len));
    x = x > 1.f ? x - pos[0] : x;
    z = z > 1.f ? z - pos[2] : z;

    face f;
    float result;
    if (x - z <= 0) {
        f.v[0] = t->v[t->f[Upperface]];
        f.v[1] = t->v[t->f[Upperface + 3]];
        f.v[2] = t->v[t->f[Upperface + 6]];
    } else {
        f.v[0] = t->v[t->f[Lowerface]];
        f.v[1] = t->v[t->f[Lowerface + 3]];
        f.v[2] = t->v[t->f[Lowerface + 6]];
    }

    const vec4f xs = { f.v[0][0],  f.v[1][0], f.v[2][0], 0};
    const vec4f zs = { f.v[0][2],  f.v[1][2], f.v[2][2], 0};

    const vec4f xmx = xs - __builtin_shuffle(xs, hmask);
    const vec4f zmz = zs - __builtin_shuffle(zs, hmask);

    const float area = (xmx[0] * zmz[1]) - (zmz[0] * xmx[1]);
    vec4f za = (((t_coords[0] - xs) * zmz) - ((t_coords[2] - zs) * xmx)) / area;

    const vec4f height = za[0] * f.v[2] + za[1] * f.v[0] + za[2] * f.v[1];

    return height[1];
}
/* Retrieves Terrain *t Quad index at given coords. */
const int getTerrainQuadIndex(Mesh *t, vec4f coords) {
    float quad_len = t->scale / tf.vecWidth;
    const int t_limit = t->scale - quad_len;

    vec4f t_coords = coords - (t->pivot - (t->scale / 2.f));
    if ( (t_coords[0] >= t_limit || t_coords[0] < 0) || (t_coords[2] >= t_limit || t_coords[2] < 0) ) {
        fprintf(stderr, "Out of terrain Limits -- getTerrainQuadIndex().\n");
        return 0;
    } 

    /* Function to get t quads indexes. */
    vec4i pos = __builtin_convertvector((t_coords / (t->scale)) * (float)tf.vecWidth, vec4i);
    const int quad_index = (pos[2] * tf.quadRows) + pos[0];

    return (pos[2] * tf.quadRows) + pos[0];
}
const void addMeshToQuad(Mesh *m) {
    int initFirst = 0;
    if (!tf.quads[m->quadIndex].mems) {
        tf.quads[m->quadIndex].mems = calloc(1, 4);
        tf.quads[m->quadIndex].mems_indexes = 1;
        initFirst = 1;
    }
    if (!initFirst) {
        for (int i = 0; i < tf.quads[m->quadIndex].mems_indexes; i++) {
            if (tf.quads[m->quadIndex].mems[i] == m->id)
                return;
        }
    }
    if (!initFirst) {
        tf.quads[m->quadIndex].mems = realloc(tf.quads[m->quadIndex].mems, (tf.quads[m->quadIndex].mems_indexes + 1) * 4);
        tf.quads[m->quadIndex].mems[tf.quads[m->quadIndex].mems_indexes + 1] = m->id;
        tf.quads[m->quadIndex].mems_indexes += 1;
        return;
    }
    tf.quads[m->quadIndex].mems[0] = m->id;
}
const void printQuad(const int quad_index) {
    for (int i = 0; i < tf.quads[quad_index].mems_indexes; i++) {
        if (!tf.quads[quad_index].mems) {
            fprintf(stdout, "Quad %d has no members.\n", quad_index);
            return;
        } else {
            if (i == 0)
                printf("Quad %d members: ", quad_index);
            printf("%d, ", tf.quads[quad_index].mems[i]);
        }
    }
    printf("\n");
}



