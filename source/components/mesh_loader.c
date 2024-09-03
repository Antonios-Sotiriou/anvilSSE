#include "../../headers/components/mesh_loader.h"

static vec4f *loadvectors(const char path[]);
static vec2f *loadtextors(const char path[]);
static vec4f *loadnormals(const char path[]);
static int *loadfaces(const char path[]);
// const static void *assemblyfaces(MeshStepOne *m, unsigned int *indices, const int len);

/* vectors array indexes */
static int v_indexes = 0;
/* normals array indexes */
static int n_indexes = 0;
/* textures array indexes */
static int t_indexes = 0;
/* Face array indexes */
static int f_indexes = 0;

const void loadMesh(Mesh *m, const char name[], const unsigned int meshlod) {
    size_t len = 17 + (strlen(name));
    char objfile[len];
    snprintf(objfile, len, "meshes/%s/lod%d.obj", name, meshlod);

    vec4f *v = loadvectors(objfile);
    if (!v)
        fprintf(stderr, "Could not reallocate Vectors array. loadMesh() - get_vectors()\n");

    vec2f *t = loadtextors(objfile);
    if (!t)
        fprintf(stderr, "Could not create Vectors array. loadMesh() - get_textors()\n");

    vec4f *n = loadnormals(objfile);
    if (!n)
        fprintf(stderr, "Could not create Vectors array. loadMesh() - get_normals()\n");

    int *f = loadfaces(objfile);
    if (!f)
        fprintf(stderr, "Could not create Faces array. loadMesh() - get_faces()\n");


    m->f_indexes = f_indexes / 9;
    m->f = malloc(sizeof(face) * m->f_indexes);

    int index = 0;
    for (int i = 0; i < f_indexes; i += 9) {
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
static int *loadfaces(const char path[]) {
    size_t facesize = sizeof(int);
    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Could not open file : %s.\n", path);
        return NULL;
    }

    int *f = malloc(facesize);
    if (!f) {
        fprintf(stderr, "Could not allocate memory for Face struct. loadMesh() -- malloc().\n");
        fclose(fp);
        return NULL;
    }
    int va, vb, vc, vd, ve, vf, vg, vh, vi;
    int dynamic_inc = 9;
    int index = 0;

    char ch;
    while (!feof(fp)) {
        if ( (ch = getc(fp)) == 'f' )
            if ( (ch = getc(fp)) == ' ' )
                if (fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d",
                    &va, &vb, &vc, &vd, &ve, &vf, &vg, &vh, &vi ) == 9){

                    f = realloc(f, facesize * dynamic_inc);
                    if (!f) {
                        fprintf(stderr, "Could not reallocate memory for Face struct array. loadMesh() -- realloc().\n");
                        fclose(fp);
                        free(f);
                        return NULL;
                    }
                    f[index]     = va - 1, f[index + 1] = vb - 1, f[index + 2] = vc - 1,
                    f[index + 3] = vd - 1, f[index + 4] = ve - 1, f[index + 5] = vf - 1,
                    f[index + 6] = vg - 1, f[index + 7] = vh - 1, f[index + 8] = vi - 1;

                    index += 9;
                    dynamic_inc += 9;
                }
    }

    f_indexes = index;
    fclose(fp);
    return f;
}
static vec4f *loadvectors(const char path[]) {
    size_t vecsize = sizeof(vec4f);
    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Could not open file : %s.\n", path);
        return NULL;
    }

    vec4f *v = malloc(vecsize);
    if (!v) {
        fprintf(stderr, "Could not allocate memory for vec4f struct. get_vectors() -- calloc().\n");
        fclose(fp);
        return NULL;
    }
    float va, vb, vc;
    int dynamic_inc = 1;
    int index = 0;

    char ch;
    while (!feof(fp)) {
        if ( (ch = getc(fp)) == 'v' )
            if ( (ch = getc(fp)) == ' ' )
                if (fscanf(fp, "%f %f %f", &va, &vb, &vc) == 3) {

                    v = realloc(v, vecsize * dynamic_inc);
                    if (!v) {
                        fprintf(stderr, "Could not reallocate memory for vec4f struct array. loadMesh() -- realloc().\n");
                        fclose(fp);
                        free(v);
                        return NULL;
                    }
                    v[index][0] = va;
                    v[index][1] = vb;
                    v[index][2] = vc;
                    v[index][3] = 1.00;

                    index++;
                    dynamic_inc++;
                }
    }
    v_indexes = index;
    fclose(fp);
    return v;
}
static vec2f *loadtextors(const char path[]) {
    size_t vecsize = sizeof(vec2f);
    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Could not open file : %s.\n", path);
        return NULL;
    }

    vec2f *t = malloc(vecsize);
    if (!t) {
        fprintf(stderr, "Could not allocate memory for Textor struct. get_textors() -- calloc().\n");
        fclose(fp);
        return NULL;
    }
    float va, vb;
    int dynamic_inc = 1;
    int index = 0;

    char ch;
    while (!feof(fp)) {
        if ( (ch = getc(fp)) == 'v' )
            if ( (ch = getc(fp)) == 't' )
                if ( (ch = getc(fp)) == ' ' )
                    if (fscanf(fp, "%f %f", &va, &vb) == 2) {

                        t = realloc(t, vecsize * dynamic_inc);
                        if (!t) {
                            fprintf(stderr, "Could not reallocate memory for vec4f struct array. get_textors() -- realloc().\n");
                            fclose(fp);
                            free(t);
                            return NULL;
                        }
                        t[index][0] = va;
                        t[index][1] = vb;

                        index++;
                        dynamic_inc++;
                    }
    }
    t_indexes = index;
    fclose(fp);
    return t;
}
static vec4f *loadnormals(const char path[]) {
    size_t vecsize = sizeof(vec4f);
    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Could not open file : %s.\n", path);
        return NULL;
    }

    vec4f *n = malloc(vecsize);
    if (!n) {
        fprintf(stderr, "Could not allocate memory for Textor struct. get_normals() -- calloc().\n");
        fclose(fp);
        return NULL;
    }

    float va, vb, vc;
    int dynamic_inc = 1;
    int index = 0;

    char ch;
    while (!feof(fp)) {
        if ( (ch = getc(fp)) == 'v' )
            if ( (ch = getc(fp)) == 'n' )
                if ( (ch = getc(fp)) == ' ' )
                    if (fscanf(fp, "%f %f %f", &va, &vb, &vc) == 3) {

                        n = realloc(n, sizeof(vec4f) * dynamic_inc);
                        if (!n) {
                            fprintf(stderr, "Could not reallocate memory for vec4f struct array. loadMesh() -- realloc().\n");
                            fclose(fp);
                            free(n);
                            return NULL;
                        }
                        n[index][0] = va;
                        n[index][1] = vb;
                        n[index][2] = vc;
                        n[index][3] = 0.0f;

                        index++;
                        dynamic_inc++;
                    }
    }
    n_indexes = index;
    fclose(fp);
    return n;
}


