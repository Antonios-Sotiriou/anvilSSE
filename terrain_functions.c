#include "headers/terrain_functions.h"

const static vec4i hmask = { 1, 2, 0, 3 };

/* Assigns the Terrain *t quad index to the given mesh. */
const void initMeshQuadInfo(Mesh *t, Mesh *m) {
    const int quad_index = getTerrainQuadIndex(t, m->pivot);
    m->quadIndex = quad_index;
    addMeshToQuad(m);
}
/* Retrieves Terrain *t height at given coords and, sets given meshes *m terain quadIndex to the id of the quad at those coords. */
const float getTerrainHeight(Mesh *t, vec4f coords, Mesh *m) {
    float quad_len = t->scale / tf.vecWidth;
    const int t_limit = t->scale - quad_len;

    vec4f t_coords = coords - (t->pivot - (t->scale / 2.f));
    if ( (t_coords[0] >= t_limit || t_coords[0] < 0) || (t_coords[2] >= t_limit || t_coords[2] < 0) ) {
        // fprintf(stderr, "Out of terrain Limits -- getTerrainHeight().\n");
        m->quadIndex = -1;
        return 0;
    } 

    /* Function to get t quads indexes. */
    vec4i pos = __builtin_convertvector((t_coords / (t->scale)) * (float)tf.vecWidth, vec4i);
    const int quad_index = (pos[2] * tf.quadRows) + pos[0];

    /* Set meshes m quadIndex to index. */
    if (m->quadIndex != quad_index && m->quadInit)
        removeMeshFromQuad(m);
    m->quadIndex = quad_index;
    m->quadInit = 1;

    /* Every quad has two faces incrementally. Every face constists of 9 indexes for vectors, normals, textors.
        So to get the right index we multiply faces with 9, because indexes are stored raw until now. */
    const int Upperface = (quad_index * 2) * 9;
    const int Lowerface = ((quad_index * 2) + 1) * 9;

    /* FInd in which triangle we are. */
    float x = ((t_coords[0] / quad_len));
    float z = ((t_coords[2] / quad_len));
    x = x >= 1.f ? x - pos[0] : x;
    z = z >= 1.f ? z - pos[2] : z;

    face f;
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
        // fprintf(stderr, "Out of terrain Limits -- getTerrainQuadIndex().\n");
        return -1;
    } 

    /* Function to get t quads indexes. */
    vec4i pos = __builtin_convertvector((t_coords / (t->scale)) * (float)tf.vecWidth, vec4i);
    const int quad_index = (pos[2] * tf.quadRows) + pos[0];

    return (pos[2] * tf.quadRows) + pos[0];
}
/* Adds a Mesh to the Quad that is standing on to, if its not already a member of this Quad. */
const void addMeshToQuad(Mesh *m) {
    const int quad_index = m->quadIndex;

    if (quad_index < 0 || !m->id) {
        /* Mesh is out of terrain if its quadIndex is less than Zero or it is the terrain if its ID is 0. */
        return;
    }

    if (!tf.quads[quad_index].mems) {
        /* Quad had not previous members in it so we must allocate memory for the new member. */
        tf.quads[quad_index].mems = calloc(1, 4);
        tf.quads[quad_index].mems_indexes = 1;
        tf.quads[quad_index].mems[0] = m->id;
        return;
    }
    for (int i = 0; i < tf.quads[quad_index].mems_indexes; i++) {
        if (tf.quads[quad_index].mems[i] == m->id)
            /* Mesh is already a member of the Quad. */
            return;
    }
    /* Increase the size of Quad members pointer to add the new member.Increment the necessery values also. */
    tf.quads[quad_index].mems = realloc(tf.quads[quad_index].mems, (tf.quads[quad_index].mems_indexes + 1) * 4);
    tf.quads[quad_index].mems[tf.quads[quad_index].mems_indexes] = m->id;
    tf.quads[quad_index].mems_indexes += 1;
}
/* Removes a mesh id from the TerrainInfo global Quad memmbers array. */
const void removeMeshFromQuad(Mesh *m) {
    const int quad_index = m->quadIndex;
    if (quad_index < 0) {
        /* Mesh is out of terrain if its quadIndex is less than Zero. */
        return;
    }
    const int num_of_indexes = tf.quads[quad_index].mems_indexes - 1;

    unsigned int *new_array = calloc(num_of_indexes, 4);

    int inc = 0;
    for (int i = 0; i < tf.quads[quad_index].mems_indexes; i++) {
        if (tf.quads[quad_index].mems[i] != m->id) {
            new_array[inc] = tf.quads[quad_index].mems[i];
            inc++;
        }
    }
    free(tf.quads[quad_index].mems);
    tf.quads[quad_index].mems = realloc(new_array, (num_of_indexes * 4));
    tf.quads[quad_index].mems_indexes = num_of_indexes;
}
/* Prints the members of given Quad index. */
const void printQuad(const int quad_index) {
    if (quad_index < 0) {
        /* Mesh is out of terrain if its quadIndex is less than Zero. */
        return;
    }
    if (!tf.quads[quad_index].mems) {
        fprintf(stderr, "Quad %d has no members.\n", quad_index);
        return;
    }
    printf("Quad %d members: ", quad_index);
    for (int i = 0; i < tf.quads[quad_index].mems_indexes; i++) {
        printf("%d, ", tf.quads[quad_index].mems[i]);
    }
    printf(": quad indexes: %d", tf.quads[quad_index].mems_indexes);
    printf("\n");
}



