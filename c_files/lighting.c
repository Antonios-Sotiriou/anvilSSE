#include "../headers/lighting.h"

const void phong(Fragment *fr) {
    vec4f basecolor;

    if (fr->mtr->texlevels) {
        basecolor = fr->mtr->texture[(fr->tex_y * fr->mtr->texture_width) + fr->tex_x];
    } else
        basecolor = fr->mtr->basecolor;

    vec4f diffuse = { 0 }, specular = { 0 };
    vec4f ambient = sunlight.material.ambient * (basecolor * AmbientStrength);
    vec4c fragcolor = { 0 };

    float w = 1.f / fr->pos[3];
    vec4f pixel = {
        ((fr->pos[0] / (point_attrib->width >> 1)) - 1.0),
        ((fr->pos[1] / (point_attrib->height >> 1)) - 1.0),
        fr->pos[2],
    };

    pixel *= w;
    pixel[3] = w;

    setvecxm(&pixel, reperspMat);
    vec4f nm = norm_vec(fr->nrm);

    /* Applying shadow test by transforming View Space coordinates to light Space. */
    vec4f lightdir = norm_vec(sunlight.newPos - pixel);
    float shadow = shadowTest(pixel);

    float diff = dot_product(lightdir, nm);
    if ( diff > 0 ) {
        diffuse = sunlight.material.diffuse * (diff * (basecolor * DiffuseStrength));

        if (fr->mtr->reflect) {
            vec4f reflectdir = cross_product(cross_product(nm, -lightdir), (-lightdir - nm)) * 2.00;
            // vec4f reflectdir = lightdir + pixel;

            float dot = dot_product(norm_vec(pixel), norm_vec(reflectdir));
            // float dot = dot_product(nm, norm_vec(reflectdir));
            if ( dot > 0 ) {
                float spec = powf(dot, fr->mtr->shinniness);
                specular = sunlight.material.specular * (spec * (basecolor * SpecularStrength));
            }
        }
    }
    fragcolor = __builtin_convertvector((((specular + diffuse) * (ambient + (1.f - shadow))) * basecolor) * 255, vec4c);

    // if (shadow == 0)
    //     fragcolor = __builtin_convertvector(((vec4f){ 0.f, 0.f, 1.f } * basecolor) * 255, vec4c);
    // else if (shadow == 1)
    //     fragcolor = __builtin_convertvector(((vec4f){ 0.f, 1.f, 0.f } * basecolor) * 255, vec4c);
    // else if (shadow == 2)
    //     fragcolor = __builtin_convertvector(((vec4f){ 1.f, 0.f, 0.f } * basecolor) * 255, vec4c);
    // else if (shadow == 3)
    //     fragcolor = __builtin_convertvector(basecolor * 255, vec4c);

    // fragcolor = __builtin_convertvector((((ambient + (1.f - shadow))) * basecolor) * 255, vec4c);
    // fragcolor = __builtin_convertvector(basecolor * 255, vec4c);

    point_frame_buffer[(int)((fr->pos[1] * point_attrib->width) + (fr->pos[0]))] = fragcolor;
}


