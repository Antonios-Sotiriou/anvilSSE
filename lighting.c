#include "headers/lighting.h"

const float phong(vec4f nm, const Material mtr, const float pixX, const float pixY, const float pixZ, const float pixW) {
    vec4f diffuse = { 0 }, specular = { 0 };
    vec4f ambient = sunlight.material.ambient * mtr.ambient;
    vec4c fragcolor = { 0 };

    float w = 1 / pixW;
    vec4f pixel = {
        ((pixX / HALFW) - 1.0) * w,
        ((pixY / HALFH) - 1.0) * w,
        (1.f / pixZ) * w,
        w
    };

    pixel = vecxm(pixel, reperspMat);

    /* Applying shadow test by transforming View Space coordinates to light Space. */
    if (!shadowTest(pixel)) {
        nm = norm_vec(nm);
        vec4f lightdir = norm_vec(sunlight.newPos - pixel);
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
        fragcolor = __builtin_convertvector(__builtin_shuffle(((specular + diffuse + ambient) * mtr.basecolor) * 255, rgbmask), vec4c);
    } else
        fragcolor = __builtin_convertvector(__builtin_shuffle((ambient * mtr.basecolor) * 255, rgbmask), vec4c);

    // fragcolor = __builtin_convertvector(__builtin_shuffle(mtr.basecolor * 255, rgbmask), vec4c);

    memcpy(&frame_buffer[(int)((pixY * wa.width * 4) + (pixX * 4))], &fragcolor, 4);
    return pixW;
}


