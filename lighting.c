#include "headers/lighting.h"

extern XWindowAttributes wa;
extern int HALFW;
extern int HALFH;
extern Mat4x4 reperspMat;
extern u_int8_t *frame_buffer;
extern float *shadow_buffer;
extern Light sunlight;
extern const int shadowTest(const vec4f frag, const vec4f nm, vec4f lightdir);

const float phong(vec4f nm, const Material mtr, const float pixX, const float pixY, const float pixZ, const float pixW) {
    vec4f diffuse = { 0 }, specular = { 0 };
    vec4f ambient = sunlight.material.ambient * mtr.ambient;
    vec4f r = { 0 };

    float w = 1 / pixW;
    vec4f pixel = {
        ((pixX / HALFW) - 1.0) * w,
        ((pixY / HALFH) - 1.0) * w,
        (1.f / pixZ) * w,
        w
    };

    pixel = vecxm(pixel, reperspMat);
    vec4f lightdir = norm_vec(sunlight.newPos - pixel);
    nm = norm_vec(nm);

    /* Applying shadow test by transforming View Space coordinates to light Space. */
    if (!shadowTest(pixel, nm, lightdir)) {
        float diff = dot_product(lightdir, nm);
        if ( diff > 0 ) {
            diffuse = sunlight.material.diffuse * (diff * mtr.diffuse);

            if (mtr.reflect) {
                vec4f reflectdir = cross_product(cross_product(nm, -lightdir), (-lightdir - nm)) * 2.00;

                float dot = dot_product(norm_vec(pixel), norm_vec(reflectdir));
                if ( dot > 0 ) {
                    float spec = powf(dot, mtr.shinniness);
                    specular = sunlight.material.specular * (spec * mtr.specular);
                }
            }
        }
        r = ((specular + diffuse + ambient) * mtr.basecolor) * 255;
    } else
        r = (ambient * mtr.basecolor) * 255;

    // r = mtr.basecolor * 255;
    // u_int8_t fragcolor[4] = { r[2], r[1], r[0], 0 };

    vec4i mask = { 2, 1, 0, 3 };
    vec4c fragcolor = __builtin_convertvector(__builtin_shuffle(r, mask), vec4c);

    memcpy(&frame_buffer[(int)((pixY * wa.width * 4) + (pixX * 4))], &fragcolor, 4);
    return pixW;
}


