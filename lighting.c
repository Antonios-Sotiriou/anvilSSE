#include "headers/lighting.h"

extern XWindowAttributes wa;
extern int HALFW;
extern int HALFH;
extern Mat4x4 reperspMat;
extern u_int8_t *frame_buffer;
extern float *shadow_buffer;
extern Light sunlight;

void phong(vec4f nm, const Material mtr, const float pixX, const float pixY, const float pixZ, const float pixW) {
    vec4f diffuse = { 0 }, specular = { 0 };
    vec4f ambient = sunlight.material.ambient * mtr.ambient;
    vec4f r = { 0 };

    float w = 1 / pixW;
    vec4f pixel;
    pixel[0] = ((pixX / HALFW) - 1.0) * w;
    pixel[1] = ((pixY / HALFH) - 1.0) * w;
    pixel[2] = (pixZ / 0.5) * w;
    pixel[3] = w;

    pixel = -vecxm(pixel, reperspMat);
    nm = norm_vec(nm);

    /* Applying shadow test by transforming View Space coordinates to light Space. */
    // vec4f shadow = shadowTest(pixel);

    vec4f lightdir = norm_vec(pixel - -(sunlight.newPos));
    float diff = dot_product(lightdir, nm);
    if ( diff > 0 ) {
        diffuse = sunlight.material.diffuse * (diff * mtr.diffuse);
        vec4f reflectdir = cross_product(cross_product(-lightdir, nm), (-lightdir - nm)) * 2.00;

        float dot = dot_product(norm_vec(pixel), norm_vec(reflectdir));
        if ( dot < 0 )
            dot = 0;

        float spec = pow(dot, mtr.shinniness);
        specular = sunlight.material.specular * (spec * mtr.specular);
    }

    r = ((specular + diffuse + ambient) * mtr.basecolor) * 255;
    u_int8_t fragcolor[4] = { r[2], r[1], r[0], 0 };

    memcpy(&frame_buffer[(int)((pixY * wa.width * 4) + (pixX * 4))], &fragcolor, 4);
    // return pixW;
}

