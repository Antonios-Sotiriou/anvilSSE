#include "../../headers/components/terrain_functions.h"

const static vec4i hmask = { 1, 2, 0, 3 };

/* Creates a Terrain from a given Height Map and Populates the TerrainInfo global struct. */
const void createTerrain(Mesh *m, const char path[], TerrainInfo *tf) {
    BMP bmp;
    int emvadon;
    char *data;

    FILE *fp;
    fp = fopen(path, "rb");

    if ( !fp ){
        fprintf(stderr, "Could not open file < %s >! readHeightmap() -- fopen().\n", path);
        return;
    } else {
        fread(&bmp.header, sizeof(BMP_Header), 1, fp);
        fseek(fp, 14, SEEK_SET);
        fread(&bmp.info, sizeof(BMP_Info), 1, fp);
        fseek(fp, bmp.header.Res2, SEEK_SET);

        emvadon = bmp.info.Height * bmp.info.Width;
        if ( !emvadon ) {
            fprintf(stderr, "Zero value for Height map Size: %d. createTerrain() --> ERROR 1\n", emvadon);
            fclose(fp);
            return;
        }
        data = malloc(emvadon);
        fread(data, emvadon, 1, fp);
    }
    fclose(fp);

    /* Populate the gloabal TerrainInfo struct with infos. */
    scene.t.vecWidth = bmp.info.Width;
    scene.t.vecHeight = bmp.info.Height;
    scene.t.quadCols = tf->vecWidth - 1;
    scene.t.quadRows = tf->vecHeight - 1;
    scene.t.quadsArea = tf->quadRows * tf->quadCols;
    scene.t.quads = calloc(tf->quadsArea, sizeof(Quad));

    /* Quads. */
    const int quad_vcols = bmp.info.Width - 1;
    const int quad_vrows = bmp.info.Height - 1;
    const int quads = quad_vrows * quad_vcols;
    if ( !quads ) {
        fprintf(stderr, "Zero value for Quads: %d. createTerrain() --> ERROR 1\n", emvadon);
        exit(1);
    }

    /* Faces. */
    const int faces_per_row = quad_vrows * 2;
    const int num_of_faces = quads * 2 * 9;

    vec4f *v = calloc(emvadon, 16);
    vec4f *n = calloc(emvadon, 16);
    vec2f *t = calloc(emvadon, 8);
    int *f = calloc(num_of_faces, 4);

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

        v[x][0] += x_step_cache;
        // v[x][1] = (float)rand() / (float)(RAND_MAX / 0.09f);
        v[x][1] = 0.f;
        //v[x][1] = data[x] / 255.f;
        v[x][2] = z_step_cache;
        v[x][3] = 1.f;

        x_step_cache += step_x;
    }
    free(data);

    /* Normals initialization. ############################## */
    vec4f normal = { 0.f, 1.f, 0.f, 0.f };
    for (int x = 0; x < emvadon; x++) {
            n[x] = normal;
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
        t[x][0] = tu_step_cache;
        t[x][1] = tv_step_cache;

        tu_step_cache += step_tu;
    }

    /* faces initialization. ############################## */
    int face_1_0 = 0;
    int face_1_1 = bmp.info.Height;
    int face_1_2 =  bmp.info.Height + 1;
    int face_counter = 0;

    for (int x = 0; x < num_of_faces; x += 18) {

        if ( face_counter == faces_per_row ) {
            face_1_0 += 1;
            face_1_1 += 1;
            face_1_2 += 1;

            face_counter = 0;
        }

        /* Face 1st Up. */
        f[x] = face_1_0;
        f[x + 1] = face_1_0;

        f[x + 3] = face_1_1;
        f[x + 4] = face_1_1;

        f[x + 6] = face_1_2;
        f[x + 7] = face_1_2;

        /* Face 2nd Down. */
        f[x + 9] = face_1_0;
        f[x + 10] = face_1_0;

        f[x + 12] = face_1_2;
        f[x + 13] = face_1_2;

        f[x + 15] = face_1_0 + 1;
        f[x + 16] = face_1_0 + 1;

        face_1_0++;
        face_1_1++;
        face_1_2++;

        face_counter += 2;
    }

    m->f_indexes = num_of_faces / 9;
    m->f = malloc(sizeof(face) * m->f_indexes);

    int index = 0;
    for (int i = 0; i < num_of_faces; i += 9) {
        m->f[index].v[0] = v[f[i]];
        m->f[index].v[1] = v[f[i + 3]];
        m->f[index].v[2] = v[f[i + 6]];

        m->f[index].vt[0] = t[f[i + 1]];
        m->f[index].vt[1] = t[f[i + 4]];
        m->f[index].vt[2] = t[f[i + 7]];

        m->f[index].vn[0] = n[f[i + 2]];
        m->f[index].vn[1] = n[f[i + 5]];
        m->f[index].vn[2] = n[f[i + 8]];
        index++;
    }

    const int vecs_num = m->f_indexes * 3;
    m->vba = malloc(vecs_num * 32);
    m->vba_indexes = vecs_num;

    int el_index = 0;
    float chunk[8];
    for (int i = 0; i < m->f_indexes; i++) {
        for (int j = 0; j < 3; j++) {

            chunk[0] = m->f[i].v[j][0];
            chunk[1] = m->f[i].v[j][1];
            chunk[2] = m->f[i].v[j][2];
            chunk[3] = m->f[i].vt[j][0];
            chunk[4] = m->f[i].vt[j][1];
            chunk[5] = m->f[i].vn[j][0];
            chunk[6] = m->f[i].vn[j][1];
            chunk[7] = m->f[i].vn[j][2];

            memcpy(&m->vba[el_index], &chunk, 32);
            el_index++;
        }
    }

    free(v);
    free(t);
    free(n);
    free(f);
}
/* Assigns the Terrain *t quad index to the given mesh. */
const void initMeshQuadInfo(Mesh *t, Mesh *m) {
    const int quad_index = getTerrainQuadIndex(t, m->cd.v[P]);
    /* Set meshes m quadIndex to index. */
    if ( m->quadIndex != quad_index && m->quadInit )
        removeMeshFromQuad(m);

    m->quadIndex = quad_index;
    m->quadInit = 1;
    addMeshToQuad(m);
}
/* Retrieves Terrain *t height at given coords and, sets given meshes *m terain quadIndex to the id of the quad at those coords. */
const TerrainPointInfo getTerrainPointData(Mesh *t, vec4f coords, Mesh *m) {
    TerrainPointInfo tp = { 0 };
    const float t_scale = t->scale * 2.f;
    float quad_len = t_scale / scene.t.vecWidth;
    const int t_limit = t_scale - quad_len;

    vec4f t_coords = coords - (t->cd.v[P] - (t->scale));
    if ( (t_coords[0] >= t_limit || t_coords[0] < 0) || (t_coords[2] >= t_limit || t_coords[2] < 0) ) {
        // fprintf(stderr, "Out of terrain Limits -- getTerrainPointData().\n");
        m->quadIndex = -1;
    } 

    /* Function to get t quads indexes. */
    vec4i pos = __builtin_convertvector((t_coords / t_scale) * (float)scene.t.vecWidth, vec4i);
    const int quad_index = (pos[2] * scene.t.quadRows) + pos[0];

    /* Every quad has two faces incrementally. Every face constists of 9 indexes for vectors, normals, textors.
        So to get the right index we multiply faces with 9, because indexes are stored raw until now. */
    const int Upperface = quad_index * 2;
    const int Lowerface = (quad_index * 2) + 1;

    /* Find in which Quad we are. */
    float x = ((t_coords[0] / quad_len));
    float z = ((t_coords[2] / quad_len));
    x = x >= 1.f ? x - pos[0] : x;
    z = z >= 1.f ? z - pos[2] : z;

    /* FInd in which face we are. */
    face f;
    if ( (x - z) <= 0 ) {
        f = t->f[Upperface];
    } else {
        f = t->f[Lowerface];
    }

    /* Translate the face from object space to world space for the edge function to work and get the interpolated height value. */
    Mat4x4 sclMatrix, trMatrix, enWorldMatrix;

    sclMatrix = scaleMatrix(t->scale);
    trMatrix = translationMatrix(t->cd.v[P][0], t->cd.v[P][1], t->cd.v[P][2]);
    enWorldMatrix = mxm(sclMatrix, trMatrix);

    f = facexm(f, enWorldMatrix);

    const vec4f xs = { f.v[0][0],  f.v[1][0], f.v[2][0], 0};
    const vec4f zs = { f.v[0][2],  f.v[1][2], f.v[2][2], 0};

    const vec4f xmx = xs - __builtin_shuffle(xs, hmask);
    const vec4f zmz = zs - __builtin_shuffle(zs, hmask);

    const float area = (xmx[0] * zmz[1]) - (zmz[0] * xmx[1]);
    vec4f za = (((t_coords[0] - xs) * zmz) - ((t_coords[2] - zs) * xmx)) / area;

    tp.pos = za[0] * f.v[2] + za[1] * f.v[0] + za[2] * f.v[1];
    tp.normal = norm_vec(triangle_cp(f));
    tp.quad_index = quad_index;

    return tp;
}
/* Retrieves Terrain *t Quad index at given coords. */
const int getTerrainQuadIndex(Mesh *t, vec4f coords) {
    const float t_scale = t->scale * 2.f;
    float quad_len = t_scale / scene.t.vecWidth;
    const int t_limit = t_scale - quad_len;

    vec4f t_coords = coords - (t->cd.v[P] - (t->scale));
    if ( (t_coords[0] >= t_limit || t_coords[0] < 0) || (t_coords[2] >= t_limit || t_coords[2] < 0) ) {
        // fprintf(stderr, "Out of terrain Limits -- getTerrainQuadIndex().\n");
        return -1;
    } 

    /* Function to get t quads indexes. */
    vec4i pos = __builtin_convertvector((t_coords / t_scale) * (float)scene.t.vecWidth, vec4i);
    const int quad_index = (pos[2] * scene.t.quadRows) + pos[0];

    return quad_index;
}
/* Adds a Mesh to the Quad that is standing on to, if its not already a member of this Quad. */
const void addMeshToQuad(Mesh *m) {
    const int quad_index = m->quadIndex;

    if ( (quad_index < 0 || !m->id) ) {
        /* Mesh is out of terrain if its quadIndex is less than Zero or it is the terrain if its ID is 0. */
        return;
    }

    if ( !scene.t.quads[quad_index].members ) {
        /* Quad had not previous members in it so we must allocate memory for the new member. */
        scene.t.quads[quad_index].members = calloc(1, 4);
        scene.t.quads[quad_index].members_indexes = 1;
        scene.t.quads[quad_index].members[0] = m->id;
        return;
    }

    for (int i = 0; i < scene.t.quads[quad_index].members_indexes; i++) {
        if ( scene.t.quads[quad_index].members[i] == m->id )
            /* Mesh is already a member of the Quad. */
            return;
    }

    /* Increase the size of Quad members pointer to add the new member.Increment the necessery values also. */
    scene.t.quads[quad_index].members = realloc(scene.t.quads[quad_index].members, (scene.t.quads[quad_index].members_indexes + 1) * 4);
    scene.t.quads[quad_index].members[scene.t.quads[quad_index].members_indexes] = m->id;
    scene.t.quads[quad_index].members_indexes += 1;
}
/* Removes a mesh id from the TerrainInfo global Quad memmbers array. */
const void removeMeshFromQuad(Mesh *m) {
    const int quad_index = m->quadIndex;

    if ( quad_index < 0 ) {
        /* Mesh is out of terrain if its quadIndex is less than Zero. */
        return;
    }
    const int num_of_indexes = scene.t.quads[quad_index].members_indexes - 1;

    int *new_array = calloc(num_of_indexes, 4);

    int inc = 0;
    for (int i = 0; i < scene.t.quads[quad_index].members_indexes; i++) {
        if ( scene.t.quads[quad_index].members[i] != m->id ) {
            new_array[inc] = scene.t.quads[quad_index].members[i];
            inc++;
        }
    }
    free(scene.t.quads[quad_index].members);
    scene.t.quads[quad_index].members = realloc(new_array, (num_of_indexes * 4));
    scene.t.quads[quad_index].members_indexes = num_of_indexes;
}


