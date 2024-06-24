#include "../headers/terrain_functions.h"

#include "../headers/logging.h"

const static vec4i hmask = { 1, 2, 0, 3 };

/* Creates a Terrain from a given Height Map and Populates the TerrainInfo global struct. */
const void createTerrain(Mesh *m, const char path[]) {
    BMP bmp;
    int emvadon;
    char *data;

    FILE *fp;
    fp = fopen(path, "rb");

    if (!fp){
        fprintf(stderr, "Could not open file < %s >! readHeightmap() -- fopen().\n", path);
        return;
    } else {
        fread(&bmp.header, sizeof(BMP_Header), 1, fp);
        fseek(fp, 14, SEEK_SET);
        fread(&bmp.info, sizeof(BMP_Info), 1, fp);
        fseek(fp, bmp.header.Res2, SEEK_SET);

        emvadon = bmp.info.Height * bmp.info.Width;
        if (!emvadon) {
            fprintf(stderr, "Zero value for Height map Size: %d. createTerrain() --> ERROR 1\n", emvadon);
            fclose(fp);
            return;
        }
        data = malloc(emvadon);
        fread(data, emvadon, 1, fp);
    }
    fclose(fp);

    /* Populate the gloabal TerrainInfo struct with infos. */
    tf.vecWidth = bmp.info.Width;
    tf.vecHeight = bmp.info.Height;
    tf.quadCols = tf.vecWidth - 1;
    tf.quadRows = tf.vecHeight - 1;
    tf.quadsArea = tf.quadRows * tf.quadCols;
    tf.quads = calloc(tf.quadsArea, sizeof(Quad));

    /* Quads. */
    const int quad_vcols = bmp.info.Width - 1;
    const int quad_vrows = bmp.info.Height - 1;
    const int quads = quad_vrows * quad_vcols;
    if (!quads) {
        fprintf(stderr, "Zero value for Quads: %d. createTerrain() --> ERROR 1\n", emvadon);
        exit(1);
    }

    /* Faces. */
    const int faces_per_row = quad_vrows * 2;
    const int num_of_faces = quads * 2 * 9;

    m->v = calloc(emvadon, 16);
    m->n = calloc(emvadon, 16);
    m->t = calloc(emvadon, 8);
    m->f = calloc(num_of_faces, 4);

    /* Vectors initialization. ############################## */ 
    float step_x = 2.f / bmp.info.Width;
    float step_z = 2.f / bmp.info.Height;
    float start_x = -1.f;//((step_x * (bmp.info.Width * 0.5)) - 1);
    float start_z = -1.f;//((step_z * (bmp.info.Height * 0.5)) - 1);

    float x_step_cache = start_x;
    float z_step_cache = start_z;

    int vcols_count = bmp.info.Width;

    for (int x = 0; x < emvadon; x++) {

        if ( x == vcols_count ) {
            x_step_cache = start_x;
            z_step_cache += step_z;

            vcols_count += bmp.info.Width;
        }

        m->v[x][0] += x_step_cache;
        // c->v[x][1] = (float)rand() / (float)(RAND_MAX / 0.09f);
        m->v[x][1] = data[x] / 255.f;
        m->v[x][2] = z_step_cache;
        m->v[x][3] = 1.f;

        x_step_cache += step_x;
    }
    free(data);

    /* Normals initialization. ############################## */
    vec4f normal = { 0.f, 1.f, 0.f, 0.f };
    for (int x = 0; x < emvadon; x++) {
            m->n[x] = normal;
    }

    /* Textors initialization. ############################## */
    float step_tu = 1.f / quad_vrows;
    float step_tv = 1.f / quad_vcols;
    float start_tu = 0.f;
    float start_tv = 0.f;
    float tu_step_cache = start_tu;
    float tv_step_cache = start_tv;

    int tx_count = bmp.info.Height;
    for (int x = 0; x < emvadon; x++) {

        if ( x == tx_count ) {
            tu_step_cache = start_tu;
            tv_step_cache += step_tv;

            tx_count += bmp.info.Height;
        }
        m->t[x][0] = tu_step_cache;
        m->t[x][1] = tv_step_cache;

        tu_step_cache += step_tu;
    }

    /* faces initialization. ############################## */
    int face_1_0 = 0;
    int face_1_1 = bmp.info.Height;
    int face_1_2 =  bmp.info.Height + 1;
    int face_counter = 0;

    for (int x = 0; x < num_of_faces; x += 18) {

        if (face_counter == faces_per_row) {
            face_1_0 += 1;
            face_1_1 += 1;
            face_1_2 += 1;

            face_counter = 0;
        }

        /* Face 1st Up. */
        m->f[x] = face_1_0;
        m->f[x + 1] = face_1_0;

        m->f[x + 3] = face_1_1;
        m->f[x + 4] = face_1_1;

        m->f[x + 6] = face_1_2;
        m->f[x + 7] = face_1_2;

        /* Face 2nd Down. */
        m->f[x + 9] = face_1_0;
        m->f[x + 10] = face_1_0;

        m->f[x + 12] = face_1_2;
        m->f[x + 13] = face_1_2;

        m->f[x + 15] = face_1_0 + 1;
        m->f[x + 16] = face_1_0 + 1;

        face_1_0++;
        face_1_1++;
        face_1_2++;

        face_counter += 2;
    }

    m->v_indexes = emvadon;
    m->n_indexes = emvadon;
    m->t_indexes = emvadon;
    m->f_indexes = num_of_faces;
}
/* Assigns the Terrain *t quad index to the given mesh. */
const void initMeshQuadInfo(Mesh *t, Mesh *m) {
    const int quad_index = getTerrainQuadIndex(t, m->pivot);
    m->quadIndex = quad_index;
    addMeshToQuad(m);
}
/* Retrieves Terrain *t height at given coords and, sets given meshes *m terain quadIndex to the id of the quad at those coords. */
const float getTerrainHeight(Mesh *t, vec4f coords, Mesh *m) {
    const float t_scale = t->scale * 2.f;
    float quad_len = t_scale / tf.vecWidth;
    const int t_limit = t_scale - quad_len;

    vec4f t_coords = coords - (t->pivot - (t->scale));
    if ( (t_coords[0] >= t_limit || t_coords[0] < 0) || (t_coords[2] >= t_limit || t_coords[2] < 0) ) {
        // fprintf(stderr, "Out of terrain Limits -- getTerrainHeight().\n");
        m->quadIndex = -1;
        return 0;
    } 
    logVec4f(t_coords);
    /* Function to get t quads indexes. */
    vec4i pos = __builtin_convertvector((t_coords / t_scale) * (float)tf.vecWidth, vec4i);
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

    /* Translate the face from object space to world space for the edge function to work and get the interpolated height value. */
    Mat4x4 sclMatrix, trMatrix, enWorldMatrix;

    sclMatrix = scaleMatrix(t->scale);
    trMatrix = translationMatrix(t->pivot[0], t->pivot[1], t->pivot[2]);
    enWorldMatrix = mxm(sclMatrix, trMatrix);

    f = facexm(f, enWorldMatrix);

    const vec4f xs = { f.v[0][0],  f.v[1][0], f.v[2][0], 0};
    const vec4f zs = { f.v[0][2],  f.v[1][2], f.v[2][2], 0};

    const vec4f xmx = xs - __builtin_shuffle(xs, hmask);
    const vec4f zmz = zs - __builtin_shuffle(zs, hmask);

    const float area = (xmx[0] * zmz[1]) - (zmz[0] * xmx[1]);
    vec4f za = (((t_coords[0] - xs) * zmz) - ((t_coords[2] - zs) * xmx)) / area;

    vec4f height = za[0] * f.v[2] + za[1] * f.v[0] + za[2] * f.v[1];

    return height[1];
}
/* Retrieves Terrain *t Quad index at given coords. */
const int getTerrainQuadIndex(Mesh *t, vec4f coords) {
    const float t_scale = t->scale * 2.f;
    float quad_len = t_scale / tf.vecWidth;
    const int t_limit = t_scale - quad_len;

    vec4f t_coords = coords - (t->pivot - (t->scale));
    if ( (t_coords[0] >= t_limit || t_coords[0] < 0) || (t_coords[2] >= t_limit || t_coords[2] < 0) ) {
        // fprintf(stderr, "Out of terrain Limits -- getTerrainQuadIndex().\n");
        return -1;
    } 

    /* Function to get t quads indexes. */
    vec4i pos = __builtin_convertvector((t_coords / t_scale) * (float)tf.vecWidth, vec4i);
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

    if (!tf.quads[quad_index].members) {
        /* Quad had not previous members in it so we must allocate memory for the new member. */
        tf.quads[quad_index].members = calloc(1, 4);
        tf.quads[quad_index].members_indexes = 1;
        tf.quads[quad_index].members[0] = m->id;
        return;
    }
    for (int i = 0; i < tf.quads[quad_index].members_indexes; i++) {
        if (tf.quads[quad_index].members[i] == m->id)
            /* Mesh is already a member of the Quad. */
            return;
    }
    /* Increase the size of Quad members pointer to add the new member.Increment the necessery values also. */
    tf.quads[quad_index].members = realloc(tf.quads[quad_index].members, (tf.quads[quad_index].members_indexes + 1) * 4);
    tf.quads[quad_index].members[tf.quads[quad_index].members_indexes] = m->id;
    tf.quads[quad_index].members_indexes += 1;
}
/* Removes a mesh id from the TerrainInfo global Quad memmbers array. */
const void removeMeshFromQuad(Mesh *m) {
    const int quad_index = m->quadIndex;
    if (quad_index < 0) {
        /* Mesh is out of terrain if its quadIndex is less than Zero. */
        return;
    }
    const int num_of_indexes = tf.quads[quad_index].members_indexes - 1;

    unsigned int *new_array = calloc(num_of_indexes, 4);

    int inc = 0;
    for (int i = 0; i < tf.quads[quad_index].members_indexes; i++) {
        if (tf.quads[quad_index].members[i] != m->id) {
            new_array[inc] = tf.quads[quad_index].members[i];
            inc++;
        }
    }
    free(tf.quads[quad_index].members);
    tf.quads[quad_index].members = realloc(new_array, (num_of_indexes * 4));
    tf.quads[quad_index].members_indexes = num_of_indexes;
}
/* Prints the members of given Quad index. */
const void printQuad(const int quad_index) {
    if (quad_index < 0) {
        /* Mesh is out of terrain if its quadIndex is less than Zero. */
        return;
    }
    if (!tf.quads[quad_index].members) {
        fprintf(stderr, "Quad %d has no members.\n", quad_index);
        return;
    }
    printf("Quad %d members: ", quad_index);
    for (int i = 0; i < tf.quads[quad_index].members_indexes; i++) {
        printf("%d, ", tf.quads[quad_index].members[i]);
    }
    printf(": quad indexes: %d", tf.quads[quad_index].members_indexes);
    printf("\n");
}



