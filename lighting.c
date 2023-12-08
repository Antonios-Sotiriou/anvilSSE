#include "headers/lighting.h"

const void phong(Fragment *fr) {
    vec4f basecolor;
    if (fr->mtr->tex_levels)
        basecolor = __builtin_convertvector(fr->mtr->texture[(fr->tex_y * fr->mtr->texture_width) + fr->tex_x], vec4f) / 255.0f;
    else
        basecolor = fr->mtr->basecolor;

    vec4f diffuse = { 0 }, specular = { 0 };
    vec4f ambient = sunlight.material.ambient * fr->mtr->ambient;
    vec4c fragcolor = { 0 };

    float w = 1 / fr->pos[3];
    vec4f pixel = {
        ((fr->pos[0] / (point_attrib->width >> 1)) - 1.0) * w,
        ((fr->pos[1] / (point_attrib->height >> 1)) - 1.0) * w,
        (1.f / fr->pos[2]) * w,
        w
    };

    pixel = vecxm(pixel, reperspMat);
    vec4f nm = norm_vec(fr->nrm);

    /* Applying shadow test by transforming View Space coordinates to light Space. */
    if (shadowTest(pixel, nm)) {
        vec4f lightdir = norm_vec(sunlight.newPos - pixel);
        float diff = dot_product(lightdir, nm);
        if ( diff > 0 ) {
            diffuse = sunlight.material.diffuse * (diff * fr->mtr->diffuse);

            if (fr->mtr->reflect) {
                vec4f reflectdir = cross_product(cross_product(nm, -lightdir), (-lightdir - nm)) * 2.00;

                float dot = dot_product(norm_vec(pixel), norm_vec(reflectdir));
                if ( dot > 0 ) {
                    float spec = powf(dot, fr->mtr->shinniness);
                    specular = sunlight.material.specular * (spec * fr->mtr->specular);
                }
            }
        }
        fragcolor = __builtin_convertvector(__builtin_shuffle((((specular + diffuse) + ambient) * basecolor) * 255, rgbmask), vec4c);
    } else
        fragcolor = __builtin_convertvector(__builtin_shuffle((ambient * basecolor) * 255, rgbmask), vec4c);

    // fragcolor = __builtin_convertvector(__builtin_shuffle(fr->mtr->basecolor * 255, rgbmask), vec4c);

    memcpy(&point_buffer[(int)((fr->pos[1] * point_attrib->width * 4) + (fr->pos[0] * 4))], &fragcolor, 4);
}


