#include "headers/obj_parser.h"

static vec4f *loadvectors(const char path[]);
static vec4f *loadtextors(const char path[]);
static vec4f *loadnormals(const char path[]);
static face *loadfaces(const char path[]);

/* vectors array indexes */
static int v_indexes = 0;
/* normals array indexes */
static int n_indexes = 0;
/* textures array indexes */
static int t_indexes = 0;
/* Face array indexes */
static int f_indexes = 0;

const Mesh loadmesh(const char path[]) {
    Mesh r = { 0 };

    r.v = loadvectors(path);
    if (!r.v)
        fprintf(stderr, "Could not reallocate Vectors array. load_obj() - get_vectors()\n");
    r.v_indexes = v_indexes;

    r.t = loadtextors(path);
    if (!r.t)
        fprintf(stderr, "Could not create Vectors array. load_obj() - get_textors()\n");
    r.t_indexes = t_indexes;

    r.n = loadnormals(path);
    if (!r.n)
        fprintf(stderr, "Could not create Vectors array. load_obj() - get_normals()\n");
    r.n_indexes = n_indexes;

    r.f = loadfaces(path);
    if (!r.f)
        fprintf(stderr, "Could not create Faces array. load_obj() - get_faces()\n");
    r.f_indexes = f_indexes;

    return r;
}
static face *loadfaces(const char path[]) {
    size_t facesize = sizeof(face);
    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Could not open file : %s.\n", path);
        return NULL;
    }

    face *f = malloc(facesize);
    if (!f) {
        fprintf(stderr, "Could not allocate memory for Face struct. load_obj() -- malloc().\n");
        fclose(fp);
        return NULL;
    }
    int dynamic_inc = 2;
    int index = 0;

    char ch;
    while (!feof(fp)) {
        if ( (ch = getc(fp)) == 'f' )
            if ( (ch = getc(fp)) == ' ' )
                if (fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d",
                    &f[index].a[0], &f[index].a[1], &f[index].a[2], 
                    &f[index].b[0], &f[index].b[1], &f[index].b[2], 
                    &f[index].c[0], &f[index].c[1], &f[index].c[2] ) == 9){

                    f = realloc(f, facesize * dynamic_inc);
                    if (!f) {
                        fprintf(stderr, "Could not reallocate memory for Face struct array. load_obj() -- realloc().\n");
                        fclose(fp);
                        free(f);
                        return NULL;
                    }
                    index++;
                    dynamic_inc++;
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

    int dynamic_inc = 2;
    int index = 0;

    char ch;
    while (!feof(fp)) {
        if ( (ch = getc(fp)) == 'v' )
            if ( (ch = getc(fp)) == ' ' )
                if (fscanf(fp, "%f %f %f", &v[index][0], &v[index][1], &v[index][2]) == 3) {

                    v = realloc(v, vecsize * dynamic_inc);
                    if (!v) {
                        fprintf(stderr, "Could not reallocate memory for vec4f struct array. load_obj() -- realloc().\n");
                        fclose(fp);
                        free(v);
                        return NULL;
                    }
                    v[index][3] = 1.00;

                    index++;
                    dynamic_inc++;
                }
    }
    v_indexes = index;
    fclose(fp);
    return v;
}
static vec4f *loadtextors(const char path[]) {
    size_t vecsize = sizeof(vec4f);
    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Could not open file : %s.\n", path);
        return NULL;
    }

    vec4f *t = malloc(vecsize);
    if (!t) {
        fprintf(stderr, "Could not allocate memory for Textor struct. get_textors() -- calloc().\n");
        fclose(fp);
        return NULL;
    }

    int dynamic_inc = 2;
    int index = 0;

    char ch;
    while (!feof(fp)) {
        if ( (ch = getc(fp)) == 'v' )
            if ( (ch = getc(fp)) == 't' )
                if ( (ch = getc(fp)) == ' ' )
                    if (fscanf(fp, "%f %f", &t[index][0], &t[index][1]) == 2) {

                        t = realloc(t, vecsize * dynamic_inc);
                        if (!t) {
                            fprintf(stderr, "Could not reallocate memory for vec4f struct array. get_textors() -- realloc().\n");
                            fclose(fp);
                            free(t);
                            return NULL;
                        }
                        t[index][2] = 0.0;
                        t[index][3] = 0.0;

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

    int dynamic_inc = 2;
    int index = 0;

    char ch;
    while (!feof(fp)) {
        if ( (ch = getc(fp)) == 'v' )
            if ( (ch = getc(fp)) == 'n' )
                if ( (ch = getc(fp)) == ' ' )
                    if (fscanf(fp, "%f %f %f", &n[index][0], &n[index][1], &n[index][2]) == 3) {

                        n = realloc(n, sizeof(vec4f) * dynamic_inc);
                        if (!n) {
                            fprintf(stderr, "Could not reallocate memory for vec4f struct array. load_obj() -- realloc().\n");
                            fclose(fp);
                            free(n);
                            return NULL;
                        }
                        n[index][3] = 0.0f;

                        index++;
                        dynamic_inc++;
                    }
    }
    n_indexes = index;
    fclose(fp);
    return n;
}
