#include "headers/obj_parser.h"

static vec4f *loadvectors(const char path[]);
static vec2f *loadtextors(const char path[]);
static vec4f *loadnormals(const char path[]);
static unsigned int *loadfaces(const char path[]);

/* vectors array indexes */
static int v_indexes = 0;
/* normals array indexes */
static int n_indexes = 0;
/* textures array indexes */
static int t_indexes = 0;
/* Face array indexes */
static int f_indexes = 0;

const void loadmesh(Mesh *m, const char name[], const unsigned int meshlod) {
    size_t len = 18 + (strlen(name) * 2) + 2;
    char objfile[len];
    snprintf(objfile, len, "objfiles/%s/%s_lod%d.obj", name, name, meshlod);

    m->v = loadvectors(objfile);
    if (!m->v)
        fprintf(stderr, "Could not reallocate Vectors array. load_obj() - get_vectors()\n");
    m->v_indexes = v_indexes;

    m->t = loadtextors(objfile);
    if (!m->t)
        fprintf(stderr, "Could not create Vectors array. load_obj() - get_textors()\n");
    m->t_indexes = t_indexes;

    m->n = loadnormals(objfile);
    if (!m->n)
        fprintf(stderr, "Could not create Vectors array. load_obj() - get_normals()\n");
    m->n_indexes = n_indexes;

    m->f = loadfaces(objfile);
    if (!m->f)
        fprintf(stderr, "Could not create Faces array. load_obj() - get_faces()\n");
    m->f_indexes = f_indexes;
}
static unsigned int *loadfaces(const char path[]) {
    size_t facesize = sizeof(unsigned int);
    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Could not open file : %s.\n", path);
        return NULL;
    }

    unsigned int *f = malloc(facesize);
    if (!f) {
        fprintf(stderr, "Could not allocate memory for Face struct. load_obj() -- malloc().\n");
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
                        fprintf(stderr, "Could not reallocate memory for Face struct array. load_obj() -- realloc().\n");
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
                        fprintf(stderr, "Could not reallocate memory for vec4f struct array. load_obj() -- realloc().\n");
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
                            fprintf(stderr, "Could not reallocate memory for vec4f struct array. load_obj() -- realloc().\n");
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


