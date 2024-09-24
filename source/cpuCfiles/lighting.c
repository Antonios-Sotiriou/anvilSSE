#include "../../headers/cpuHfiles/lighting.h"

const static vec4f add_one = { 1.f, 1.f, 0.f, 0.f };

const void phong(Fragment *fr) {
    int tex_y = (fr->tex[1] * fr->mtr->texture_height) / fr->pos[3];
    int tex_x = (fr->tex[0] * fr->mtr->texture_width) / fr->pos[3];

    vec4f basecolor = fr->mtr->texlevels ? fr->mtr->texture[(tex_y * fr->mtr->texture_width) + tex_x] : fr->mtr->basecolor;

    vec4f diffuse = { 0 }, specular = { 0 };
    vec4f ambient = scene.m[7].material.ambient * (basecolor * AmbientStrength);
    vec4c fragcolor = { 0 };

    /* Transform fragment back to Clip Space */
    float w = 1.f / fr->pos[3];
    vec4f pixel = ((fr->pos / __builtin_convertvector(half_screen, vec4f)) - add_one) * w;
    pixel[3] = w;

    /* Transform fragment back to View Space */
    setvecxm(&pixel, reperspMatrix);

    /* Transform fragment back to World Space */
    pixel[3] = 1.f;
    setvecxm(&pixel, *point_mat);
    fr->nrm = norm_vec(fr->nrm);

    /* Applying shadow test by transforming View Space coordinates to light Space. */
    vec4f lightdir = norm_vec(scene.m[7].cd.v[P] - pixel);
    vec4f viewdir = norm_vec(scene.m[6].cd.v[P] - pixel);

    float shadow = shadowTest(pixel);
    float diff = dot_product(lightdir, fr->nrm);

    if ( diff > 0 ) {
        diffuse = scene.m[7].material.diffuse * (diff * (basecolor * DiffuseStrength));

        if (fr->mtr->reflect) {
            vec4f reflectdir = cross_product(cross_product(fr->nrm, -lightdir), (-lightdir - fr->nrm)) * 2.00;
            // vec4f reflectdir = lightdir + viewdir;

            float dot = dot_product(-viewdir, norm_vec(reflectdir));
            // float dot = dot_product(fr->nrm, norm_vec(reflectdir));
            if ( dot > 0 ) {
                float spec = powf(dot, fr->mtr->shinniness);
                specular = scene.m[7].material.specular * (spec * (basecolor * SpecularStrength));
            }
        }
    }
    fragcolor = __builtin_convertvector((((specular + diffuse) * (ambient + (1.f - shadow))) * basecolor) * 255, vec4c);
    // fragcolor = __builtin_convertvector(((specular + diffuse + ambient) * basecolor) * 255, vec4c);

    // if (shadow == 0)
    //     fragcolor = __builtin_convertvector(((vec4f){ 0.f, 0.f, 1.f } * basecolor) * 255, vec4c);
    // else if (shadow == 1)
    //     fragcolor = __builtin_convertvector(((vec4f){ 0.f, 1.f, 0.f } * basecolor) * 255, vec4c);
    // else if (shadow == 2)
    //     fragcolor = __builtin_convertvector(((vec4f){ 1.f, 0.f, 0.f } * basecolor) * 255, vec4c);
    // else if (shadow == 3)
    //     fragcolor = __builtin_convertvector(((vec4f){ 0.f, 1.f, 0.f } * basecolor) * 255, vec4c);

    // fragcolor = __builtin_convertvector((((ambient + (1.f - shadow))) * basecolor) * 255, vec4c);
    // fragcolor = __builtin_convertvector(basecolor * 255, vec4c);

    point_frame_buffer[(int)((fr->pos[1] * point_attrib->width) + (fr->pos[0]))] = fragcolor;
}


