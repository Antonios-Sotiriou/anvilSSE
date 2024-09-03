#include "../../headers/components/database.h"

const int createMaterialDatabase(void) {
    FILE *fp = fopen("tables/materials.dat", "rb");
    if (fp) {
        fprintf(stderr, "tables/materials.dat... OK!\n");
        fclose(fp);
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "INFO: data/materials.dat not found... Creating!\n");
        fp = fopen("tables/materials.dat", "wb");
    }

    const int intensity = 128;
    Material mats[31] = {
        {
            .name = "skybox",
            .basecolor = { 1.0f, 0.8f, 0.0f, 0.0f },
            .ambient = { 0.24725f, 0.1995f, 0.0745f, 0.0f },
            .specular = { 0.628281f, 0.555802f, 0.366065f, 0.0f },
            .diffuse = { 0.75164f, 0.60648f, 0.22648f, 0.0f },
            .shinniness = 0.4f * intensity,
            .reflect = 0,
            .texlevels = 1,
            .texlvl = { "2048x1536", "2048x1536", "2048x1536", "2048x1536", "2048x1536", "2048x1536", "2048x1536", "2048x1536", "2048x1536" }
        },
        {
            .name = "space",
            .basecolor = { 1.0f, 0.8f, 0.0f, 0.0f },
            .ambient = { 0.24725f, 0.1995f, 0.0745f, 0.0f },
            .specular = { 0.628281f, 0.555802f, 0.366065f, 0.0f },
            .diffuse = { 0.75164f, 0.60648f, 0.22648f, 0.0f },
            .shinniness = 0.4f * intensity,
            .reflect = 0,
            .texlevels = 1,
            .texlvl = { "3840x2160", "3840x2160", "3840x2160", "3840x2160", "3840x2160", "3840x2160", "3840x2160", "3840x2160", "3840x2160" }
        },
        {
            .name = "stones",
            .basecolor = { 1.0f, 0.8f, 0.0f, 0.0f },
            .ambient = { 0.24725f, 0.1995f, 0.0745f, 0.0f },
            .specular = { 0.628281f, 0.555802f, 0.366065f, 0.0f },
            .diffuse = { 0.75164f, 0.60648f, 0.22648f, 0.0f },
            .shinniness = 0.4f * intensity,
            .reflect = 0,
            .texlevels = 9,
            .texlvl = { "2048x2048", "1024x1024", "512x512", "256x256", "128x128", "64x64", "32x32", "16x16", "8x8" }
        },
        {
            .name = "earth",
            .basecolor = { 1.0f, 0.8f, 0.0f, 0.0f },
            .ambient = { 0.24725f, 0.1995f, 0.0745f, 0.0f },
            .diffuse = { 0.75164f, 0.60648f, 0.22648f, 0.0f },
            .specular = { 0.628281f, 0.555802f, 0.366065f, 0.0f },
            .shinniness = 0.4f * intensity,
            .reflect = 0,
            .texlevels = 9,
            .texlvl = { "4096x2048", "2048x1024", "1024x512", "512x256", "256x128", "128x64", "64x32", "32x16", "16x8" }
        },
        {
            .name = "greece",
            .basecolor = { 1.0f, 0.8f, 0.0f, 0.0f },
            .ambient = { 0.24725f, 0.1995f, 0.0745f, 0.0f },
            .diffuse = { 0.75164f, 0.60648f, 0.22648f, 0.0f },
            .specular = { 0.628281f, 0.555802f, 0.366065f, 0.0f },
            .shinniness = 0.4f * intensity,
            .reflect = 0,
            .texlevels = 1,
            .texlvl = { "1640x1376", "1640x1376", "1640x1376", "1640x1376", "1640x1376", "1640x1376", "1640x1376", "1640x1376", "1640x1376" }
        },
        {
            .name = "sunlight",
            .basecolor = { 1.0f, 1.0f, 1.0f, 1.0f },
            .ambient = { 0.5f, 0.5f, 0.5f, 1.0f },
            .diffuse = { 1.0f, 1.0f, 1.0f, 1.0f },
            .specular = { 0.5f, 0.5f, 0.5f, 1.0f },
            .shinniness = 0.4f * intensity,
            .reflect = 0,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "light",
            .basecolor = { 1.0f, 0.8f, 0.0f, 0.0f },
            .ambient = { 0.24725f, 0.1995f, 0.0745f, 0.0f },
            .diffuse = { 0.75164f, 0.60648f, 0.22648f, 0.0f },
            .specular = { 0.628281f, 0.555802f, 0.366065f, 0.0f },
            .shinniness = 0.4f * intensity,
            .reflect = 0,
            .texlevels = 7,
            .texlvl = { "1080x562", "1080x562", "1080x562", "540x281", "270x140", "135x70", "68x35", "34x17", "17x8" }
        },
        {
            .name = "gold", // There is a bug which leads to a segmentation EROOR with 1x1 resolution textures.
            .basecolor = { 1.0f, 0.8f, 0.0f, 0.0f },
            .ambient = { 0.24725f, 0.1995f, 0.0745f, 0.0f },
            .diffuse = { 0.75164f, 0.60648f, 0.22648f, 0.0f },
            .specular = { 0.628281f, 0.555802f, 0.366065f, 0.0f },
            .shinniness = 0.4f * intensity,
            .reflect = 1,
            .texlevels = 1,
            .texlvl = { "1x1", "1x1", "1x1", "1x1", "1x1", "1x1", "1x1", "1x1", "1x1" }
        },
        {
            .name = "emerald",
            .basecolor = { 0.31372549f, 0.784313725f, 0.470588235f, 0.0f },
            .ambient = { 0.0215f, 0.1745f, 0.0215f, 0.0f },
            .diffuse = { 0.07568f, 0.61424f, 0.07568f, 0.0f },
            .specular = { 0.633f, 0.727811f, 0.633f, 0.0f },
            .shinniness = 0.6f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "jade",
            .basecolor = { 0.0f, 0.658823529f, 0.419607843f, 0.0f },
            .ambient = { 0.135f, 0.2225f, 0.1575f, 0.0f },
            .diffuse = { 0.54f, 0.89f, 0.63f, 0.0f },
            .specular = { 0.316228f, 0.316228f, 0.316228f, 0.0f },
            .shinniness = 0.1f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "obsidian",
            .basecolor = { 0.443137255f, 0.384313725f, 0.478431373f, 0.0f },
            .ambient = { 0.05375f, 0.05f, 0.06625f, 0.0f },
            .diffuse = { 0.18275f, 0.17f, 0.22525f, 0.0f },
            .specular = { 0.332741f, 0.328634f, 0.346435f, 0.0f },
            .shinniness = 0.3f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "pearl",
            .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.25f, 0.20725f, 0.20725f, 0.0f },
            .diffuse = { 1.f, 0.829f, 0.829f, 0.0f },
            .specular = { 0.296648f, 0.296648f, 0.296648f, 0.0f },
            .shinniness = 0.088f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "ruby",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.1745f, 0.01175f, 0.01175f, 0.0f },
            .diffuse = { 0.61424f, 0.04136f, 0.04136f, 0.0f },
            .specular = { 0.727811f, 0.626959f, 0.626959f, 0.0f },
            .shinniness = 0.6f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "turquoise",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.1f, 0.18725f, 0.1745f, 0.0f },
            .diffuse = { 0.396f, 0.74151f, 0.69102f, 0.0f },
            .specular = { 0.297254f,0.30829f, 0.306678f, 0.0f },
            .shinniness = 0.1f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "brass",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.329412f, 0.223529f, 0.027451f , 0.0f },
            .diffuse = { 0.780392f, 0.568627f, 0.113725f, 0.0f },
            .specular = { 0.992157f,0.941176f, 0.807843f, 0.0f },
            .shinniness = 0.21794872f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "bronze",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.2125f, 0.1275f, 0.054f, 0.0f },
            .diffuse = { 0.714f, 0.4284f, 0.18144f, 0.0f },
            .specular = { 0.393548f,0.271906f, 0.166721f, 0.0f },
            .shinniness = 0.2f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "chrome",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.25f, 0.25f, 0.25f, 0.0f },
            .diffuse = { 0.4f, 0.4f, 0.4f, 0.0f },
            .specular = { 0.774597f,0.774597f, 0.774597f, 0.0f },
            .shinniness = 0.6f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "copper",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.19125f, 0.0735f, 0.0225f, 0.0f },
            .diffuse = { 0.7038f, 0.27048f, 0.0828f, 0.0f },
            .specular = { 0.256777f,0.137622f, 0.086014f, 0.0f },
            .shinniness = 0.1f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "silver",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.19225f, 0.19225f, 0.19225f, 0.0f },
            .diffuse = { 0.50754f, 0.50754f, 0.50754f, 0.0f },
            .specular = { 0.508273f,0.508273f, 0.508273f, 0.0f },
            .shinniness = 0.4f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "black_plastic",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.0f, 0.0f, 0.0f, 0.0f },
            .diffuse = { 0.01f, 0.01f, 0.01f, 0.0f },
            .specular = { 0.50f,0.50f, 0.50f , 0.0f },
            .shinniness = 0.25f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "cyan_plastic",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.0f, 0.1f, 0.06f, 0.0f },
            .diffuse = { 0.0f, 0.50980392f, 0.50980392f, 0.0f },
            .specular = { 0.50196078f,0.50196078f, 0.50196078f, 0.0f },
            .shinniness = 0.25f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "green_plastic",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.0f, 0.0f, 0.0f, 0.0f },
            .diffuse = { 0.1f, 0.35f, 0.1f, 0.0f },
            .specular = { 0.45f,0.55f, 0.45f, 0.0f },
            .shinniness = 0.25f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "red_plastic",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.0f, 0.0f, 0.0f, 0.0f },
            .diffuse = { 0.5f, 0.0f, 0.0f, 0.0f },
            .specular = { 0.7f,0.6f, 0.6f, 0.0f },
            .shinniness = 0.25f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "white_plastic",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.0f, 0.0f, 0.0f, 0.0f },
            .diffuse = { 0.55f, 0.55f, 0.55f, 0.0f },
            .specular = { 0.70f,0.70f, 0.70f, 0.0f },
            .shinniness = 0.25f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "yellow_plastic",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.0f, 0.0f, 0.0f, 0.0f },
            .diffuse = { 0.5f, 0.5f, 0.0f, 0.0f },
            .specular = { 0.60f,0.60f, 0.50f, 0.0f },
            .shinniness = 0.25f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "black_rubber",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.02f, 0.02f, 0.02f, 0.0f },
            .diffuse = { 0.01f, 0.01f, 0.01f, 0.0f },
            .specular = { 0.4f,0.4f, 0.4f, 0.0f },
            .shinniness = 0.078125f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "cyan_rubber",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.0f, 0.05f, 0.05f, 0.0f },
            .diffuse = { 0.4f, 0.5f, 0.5f, 0.0f },
            .specular = { 0.04f,0.7f, 0.7f, 0.0f },
            .shinniness = 0.078125f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "green_rubber",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.0f, 0.05f, 0.0f, 0.0f },
            .diffuse = { 0.4f, 0.5f, 0.4f, 0.0f },
            .specular = { 0.04f,0.7f, 0.04f, 0.0f },
            .shinniness = 0.078125f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "red_rubber",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.05f, 0.0f, 0.0f, 0.0f },
            .diffuse = { 0.5f, 0.4f, 0.4f, 0.0f },
            .specular = { 0.7f,0.04f, 0.04f, 0.0f },
            .shinniness = 0.078125f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "white_rubber",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.05f, 0.05f, 0.05f, 0.0f },
            .diffuse = { 0.5f, 0.5f, 0.5f, 0.0f },
            .specular = { 0.7f,0.7f, 0.7f, 0.0f },
            .shinniness = 0.078125f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
        {
            .name = "yellow_rubber",
            // .basecolor = { 0.960784314f, 0.921568627f, 0.854901961f, 0.0f },
            .ambient = { 0.05f, 0.05f, 0.0f, 0.0f },
            .diffuse = { 0.5f, 0.5f, 0.4f, 0.0f },
            .specular = { 0.7f,0.7f, 0.04f, 0.0f },
            .shinniness = 0.078125f * intensity,
            .reflect = 1,
            .texlevels = 0,
            .texlvl = { { 0 } }
        },
    };
    
    for (int i = 0; i < 31; i++)
        fwrite(&mats[i], sizeof(Material), 1, fp);

    fprintf(stderr, "INFO: tables/materials.dat... Success!\n");
    fclose(fp);
    return EXIT_SUCCESS;
}


