#include "../headers/bbox_loader.h"

static vec4f *loadvectors(const char path[]);

/* vectors array indexes */
static int v_indexes = 0;

const void loadBbox(Bbox *b, const char path[]) {
    size_t len = 5 + (strlen(path));
    char objfile[len];
    snprintf(objfile, len, "%s.obj", path);

    b->v = loadvectors(objfile);
    if (!b->v)
        fprintf(stderr, "Could not reallocate Vectors array. load_obj() - get_vectors()\n");
    b->v_indexes = v_indexes;
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

