#include "headers/height_pipeline.h"

const static float sortVertices(face *f, vec4f pivot);
const static float checkIfInside(face *f, vec4i pv);
const static vec4i hmask = { 1, 2, 0, 3 };
#include "headers/logging.h"
const float getTerrainHeight(Scene *s, vec4f *pivot) {
    face temp;
    float height = 0;
    // system("clear\n");
    for (int i = 0; i < s->m_indexes; i++) {
        if (s->m[i].type == Terrain) {
            for (int j = 0; j < s->m[i].f_indexes; j += 9) {

                temp.v[0] = s->m[i].v[s->m[i].f[j]];
                temp.v[1] = s->m[i].v[s->m[i].f[j + 3]];
                temp.v[2] = s->m[i].v[s->m[i].f[j + 6]];

                height = sortVertices(&temp, *pivot);
                if (height > 0)
                    return height;
            }
        }
    }
    return 0;
}
const static float sortVertices(face *f, vec4f pivot) {
    vec4i pv = __builtin_convertvector(pivot + 0.5, vec4i);
    return checkIfInside(f, pv);
}
const static float checkIfInside(face *f, vec4i pv) {
    const vec4i xs = { f->v[0][0],  f->v[1][0], f->v[2][0], 0};
    const vec4i zs = { f->v[0][2],  f->v[1][2], f->v[2][2], 0};

    const vec4i xmx = xs - __builtin_shuffle(xs, hmask);
    const vec4i zmz = zs - __builtin_shuffle(zs, hmask);

    const int tps0 = ((zmz[0] == 0) && (zs[1] > zs[2])) || (zmz[0] < 0) ? 1 : 0;
    const int tps1 = ((zmz[1] == 0) && (zs[2] > zs[0])) || (zmz[1] < 0) ? 1 : 0;
    const int tps2 = ((zmz[2] == 0) && (zs[0] > zs[1])) || (zmz[2] < 0) ? 1 : 0;

    const float area = ((xs[0] - xs[2]) * zmz[1]) - ((zs[0] - zs[2]) * xmx[1]);
    vec4i za = ((pv[0] - xs) * zmz) - ((pv[2] - zs) * xmx);
    // printf("Inside: area: %f\n", area);
    // logVec4i(za);
    // printf("tsp0: %d, tps1: %d, tps2: %d\n", tps0, tps1, tps2);

    const int xa0 = ( (tps0) && (!za[0]) ) ? -1 : za[0];
    const int xa1 = ( (tps1) && (!za[1]) ) ? -1 : za[1];
    const int xa2 = ( (tps2) && (!za[2]) ) ? -1 : za[2];
    // logFace(*f, 1, 0, 0);
    if ( (xa0 | xa1 | xa2) > 0 ) {
        const vec4f a = __builtin_convertvector(za, vec4f) / area;
        // logVec4i(a);
        const vec4f height = a[0] * f->v[1] + a[1] * f->v[2] + a[2] * f->v[0];
        // printf("Inside: height = %f\n", height[1] * -1);
        // logVec4f(a); 
        return height[1] * -1;
    }
    // printf("Outside\n");
    return 0;
}
const float getTerrainHeightTest(Mesh *terrain, vec4f coords) {
    int quads = 99;
    float quad_len = terrain->scale / 100;
    const int terrain_limit = terrain->scale - quad_len;

    vec4f terrain_coords = coords - (terrain->pivot - (terrain->scale / 2.f));
    if ( (terrain_coords[0] >= terrain_limit || terrain_coords[0] < 0) || (terrain_coords[2] >= terrain_limit || terrain_coords[2] < 0) ) {
        printf("outofLimits\n");
        return 0;
    } 
    logVec4f(terrain_coords);

    /* Function to get terrain quads indexes. */
    vec4i pos = __builtin_convertvector((terrain_coords / (terrain->scale)) * 100, vec4i);
    const int quad_index = (pos[2] * 99) + pos[0];

    /* Every quad has two faces incrementally. Every face constists of 9 indexes for vectors, normals, textors.
        So to get the right index we multiply faces with 9, because indexes are stored raw until now. */
    const int Upperface = (quad_index * 2) * 9;
    const int Lowerface = ((quad_index * 2) + 1) * 9;
    // printf("Quad index: %d\n", quad_index);
    // printf("Tri index: %d,    %d\n", Upperface, Lowerface);

    /* FInd in which triangle we are. */
    float x = ((terrain_coords[0] / quad_len));
    float z = ((terrain_coords[2] / quad_len));
    x = x > 1.f ? x - pos[0] : x;
    z = z > 1.f ? z - pos[2] : z;

    face f;
    float result;
    if (x - z <= 0) {
        // printf("Upper\n");
        f.v[0] = terrain->v[terrain->f[Upperface]];
        f.v[1] = terrain->v[terrain->f[Upperface + 3]];
        f.v[2] = terrain->v[terrain->f[Upperface + 6]];
    } else {
        // printf("Lower:\n");
        f.v[0] = terrain->v[terrain->f[Lowerface]];
        f.v[1] = terrain->v[terrain->f[Lowerface + 3]];
        f.v[2] = terrain->v[terrain->f[Lowerface + 6]];
    }

    const vec4f xs = { f.v[0][0],  f.v[1][0], f.v[2][0], 0};
    const vec4f zs = { f.v[0][2],  f.v[1][2], f.v[2][2], 0};

    const vec4f xmx = xs - __builtin_shuffle(xs, hmask);
    const vec4f zmz = zs - __builtin_shuffle(zs, hmask);

    const float area = (xmx[0] * zmz[1]) - (zmz[0] * xmx[1]);
    vec4f za = (((terrain_coords[0] - xs) * zmz) - ((terrain_coords[2] - zs) * xmx)) / area;

    const vec4f height = za[0] * f.v[2] + za[1] * f.v[0] + za[2] * f.v[1];

    // printf("x: %f    z: %f\n", x, z);
    // printf("x: %f    z: %f\n", terrain_x, terrain_z);

    // printf("height = %f\n", height[1]);
    return height[1];
    // logVec4i(pos);
}



