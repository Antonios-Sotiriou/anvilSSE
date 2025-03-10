#include "../../headers/components/logging.h"

const void logEvent(const XEvent e) {
    printf("Event {\n");
    fprintf(stdout, "    type        : %d\n", e.type);
    // fprintf(stdout, "    pad         : %ln\n", e.pad);
    fprintf(stdout, "    display     : %p\n", e.xkey.display);
    fprintf(stdout, "    keycode     : %d\n", e.xkey.keycode);
    fprintf(stdout, "    root        : %ld\n", e.xkey.root);
    fprintf(stdout, "    same_screen : %d\n", e.xkey.same_screen);
    fprintf(stdout, "    send_event  : %d\n", e.xkey.send_event);
    fprintf(stdout, "    serial      : %ld\n", e.xkey.serial);
    fprintf(stdout, "    state       : %d\n", e.xkey.state);
    fprintf(stdout, "    subwindow   : %ld\n", e.xkey.subwindow);
    fprintf(stdout, "    time        : %ld\n", e.xkey.time);
    fprintf(stdout, "    type        : %d\n", e.xkey.type);
    fprintf(stdout, "    window      : %ld\n", e.xkey.window);
    fprintf(stdout, "    x           : %d\n", e.xkey.x);
    fprintf(stdout, "    y           : %d\n", e.xkey.y);
    fprintf(stdout, "    x_root      : %d\n", e.xkey.x_root);
    fprintf(stdout, "    y_root      : %d\n", e.xkey.y_root);
    printf("}\n");
}

const void logVec4f(const vec4f v) {
    fprintf(stdout, "x: %f    y: %f    z: %f    w: %f\n", v[0], v[1], v[2], v[3]);
}

const void logVec4i(const vec4i v) {
    fprintf(stdout, "x: %d    y: %d    z: %d    w: %d\n", v[0], v[1], v[2], v[3]);
}

const void logVec4c(const vec4c v) {
    fprintf(stdout, "x: %d    y: %d    z: %d    w: %d\n", v[0], v[1], v[2], v[3]);
}

const void logVec2f(const vec2f v) {
    fprintf(stdout, "u: %f    v: %f\n", v[0], v[1]);
}

/* Logging Mesh. */
const void logMesh(const Mesh m) {
    printf("Mesh id:       %d\n", m.id);
    printf("Mesh name:     %s\n", m.name);
    printf("cd.v[P]:       ");
    logVec4f(m.cd.v[P]);
    printf("cd.v[U]:       ");
    logVec4f(m.cd.v[U]);
    printf("cd.v[V]:       ");
    logVec4f(m.cd.v[V]);
    printf("cd.v[N]:       ");
    logVec4f(m.cd.v[N]);
    printf("f:             %p -->  ", m.f);
    printf("f_indexes:     %d\n", m.f_indexes);
    printf("Quaternion:    ");
    logQuat(m.Q);
    printf("Last Rotation: ");
    logQuat(m.r);
    printf("cull:          %d\n", m.cull);
    printf("lodlevels:     %d\n", m.lodlevels);
    printf("meshlod:       %d\n", m.currentlod);
    printf("scale:         %f\n", m.scale);
    printf("visible:       %d\n", m.visible);
    printf("grounded:      %d\n", m.grounded);
    printf("quadIndex:     %d\n", m.quadIndex);
    printf("bboxv_indexes: %d\n", m.bbox.v_indexes);
    logMaterial(m.material);
}

const void logMaterial(const Material mt) {
        printf("Material: {\n");
        printf("    name          : %s\n", mt.name);
        printf("    basecolor     : ");
        logVec4f(mt.basecolor);
        printf("    ambient       : ");
        logVec4f(mt.ambient);
        printf("    diffuse       : ");
        logVec4f(mt.diffuse);
        printf("    specular      : ");
        logVec4f(mt.specular);
        printf("    shinniness    : %f\n", mt.shinniness);
        printf("    reflect       : %d\n", mt.reflect);
        printf("    texlevels     : %d\n", mt.texlevels);
        printf("    texlod        : %d\n", mt.texlod);
        printf("    texture_height: %d\n", mt.texture_height);
        printf("    texture_width : %d\n", mt.texture_width);
        printf("    texture       : %p\n", mt.texture);
        printf("    texlvl[9][10] : ");
        for (int i = 0; i < 9; i++) {
            printf("%s, ", mt.texlvl[i]);
        }
        printf("\n");
        printf("    Initialized   : %d\n", mt.init);
        printf("}\n");
}

/* Logging Triangle values.If vec = 1 loggs vec4 values, if tex Texture, if norm Normal. */
const void logFace(const face f, const int vec, const int tex, const int norm) {
    if (vec) {
        fprintf(stdout, "v0 - { %f, %f, %f, %f }\n", f.v[0][0], f.v[0][1], f.v[0][2], f.v[0][3]);
        fprintf(stdout, "v1 - { %f, %f, %f, %f }\n", f.v[1][0], f.v[1][1], f.v[1][2], f.v[1][3]);
        fprintf(stdout, "v2 - { %f, %f, %f, %f }\n", f.v[2][0], f.v[2][1], f.v[2][2], f.v[2][3]);
    } else if (tex) {
        fprintf(stdout, "f.vt[0].u: %f    f.vt[0].v: %f\n", f.vt[0][0], f.vt[0][1]);
        fprintf(stdout, "f.vt[1].u: %f    f.vt[1].v: %f\n", f.vt[1][0], f.vt[1][1]);
        fprintf(stdout, "f.vt[2].u: %f    f.vt[2].v: %f\n\n", f.vt[2][0], f.vt[2][1]);
    } else if (norm) {
        fprintf(stdout, "v0 - { %f, %f, %f, %f }\n", f.vn[0][0], f.vn[0][1], f.vn[0][2], f.vn[0][3]);
        fprintf(stdout, "v1 - { %f, %f, %f, %f }\n", f.vn[1][0], f.vn[1][1], f.vn[1][2], f.vn[1][3]);
        fprintf(stdout, "v2 - { %f, %f, %f, %f }\n", f.vn[2][0], f.vn[2][1], f.vn[2][2], f.vn[2][3]);
    }
}

const void logMatrix(const Mat4x4 m) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            fprintf(stdout, "m.m[%d][%d]: %.6f    ", i, j, m.m[i][j]);
        }
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "\n");
}

const void logQuat(const Quat q) {
    fprintf(stdout, "w: %f  v[x: %f, y: %f, z: %f]\n", q.w, q.v[0], q.v[1], q.v[2]);
}

/* Prints the members of given Quad index. */
const void logTerrainQuad(const TerrainInfo ti, const int quad_index) {
    if (quad_index < 0) {
        /* Mesh is out of terrain if its quadIndex is less than Zero. */
        return;
    }

    printf("Quad: %d\n", quad_index);
    printf("indexes: %d\n", ti.quads[quad_index].members_indexes);
    if ( !ti.quads[quad_index].members ) {
        fprintf(stderr, "Quad %d has no members.\n", quad_index);
        return;
    }
    printf("members ids: ");
    for (int i = 0; i < ti.quads[quad_index].members_indexes; i++) {
        printf("%d, ", ti.quads[quad_index].members[i]);
    }
    printf("\n");
}

/* Prints the TerrainInfo struct. */
const void logTerrainInfo(const TerrainInfo ti) {
    printf("vecWidth : %d\n", ti.vecWidth);
    printf("vecHeight: %d\n", ti.vecHeight);
    printf("quadRows : %d\n", ti.quadRows);
    printf("quadCols : %d\n", ti.quadCols);
    printf("quadsArea: %d\n", ti.quadsArea);
}

/* Prints the DimensionsLimits struct. */
const void logDm(const DimensionsLimits dm) {
    printf("dm min: ");
    logVec4f(dm.min);
    printf("dm max: ");
    logVec4f(dm.max);
}

const void logBMP_Header(const BMP_Header bmp_h) {
        printf("header.Type  : %d\n", bmp_h.Type);
        printf("header.Size  : %d\n", bmp_h.Size);
        printf("header.Res1  : %d\n", bmp_h.Res1);
        printf("header.Res2  : %d\n", bmp_h.Res2);
        printf("header.Offset: %d\n\n", bmp_h.OffSet);
}

const void logBMP_EndPoints(const CS_EndPoints cse) {
        printf("Endpoints: {\n");
        printf("    redX  : %d    redY  : %d    redZ  : %d\n", cse.red_coords.RedX, cse.red_coords.RedY, cse.red_coords.RedZ);
        printf("    GreenX: %d    GreenY: %d    GreenZ: %d\n", cse.green_coords.GreenX, cse.green_coords.GreenY, cse.green_coords.GreenZ);
        printf("    BLueX : %d    BLueY : %d    BLueZ : %d\n", cse.blue_coords.BlueX, cse.blue_coords.BlueY, cse.blue_coords.BlueZ);
        printf("}\n");
}

const void logBMP_Info(const BMP_Info bmp_i) {
        printf("info.Size            : %d\n", bmp_i.Size);
        printf("info.Width           : %d\n", bmp_i.Width);
        printf("info.Height          : %d\n", bmp_i.Height);
        printf("info.Planes          : %d\n", bmp_i.Planes);
        printf("info.BitsPerPixel    : %d\n", bmp_i.BitsPerPixel);
        printf("info.compression     : %d\n", bmp_i.compression);
        printf("info.SizeOfBitmap    : %d\n", bmp_i.SizeOfBitmap);
        printf("info.HorzResolution  : %ld\n", bmp_i.HorzResolution);
        printf("info.VertResolution  : %ld\n", bmp_i.VertResolution);
        printf("info.ColorsUsed      : %d\n", bmp_i.ColorsUsed);
        printf("info.ColorsImportand : %d\n", bmp_i.ColorsImportant);
        printf("info.RedMask         : %d\n", bmp_i.RedMask);
        printf("info.GreenMask       : %d\n", bmp_i.GreenMask);
        printf("info.BlueMask        : %d\n", bmp_i.BlueMask);
        printf("info.AlphaMask       : %d\n", bmp_i.AlphaMask);
        printf("info.CSType          : %d\n", bmp_i.CSType);
        logBMP_EndPoints(bmp_i.CSEndPoints);
        printf("info.GammaRed        : %d\n", bmp_i.GammaRed);
        printf("info.GammaGreen      : %d\n", bmp_i.GammaGreen);
        printf("info.GammaBlue       : %d\n", bmp_i.GammaBlue);
        printf("info.Intent          : %d\n", bmp_i.Intent);
        printf("info.ProfileData     : %d\n", bmp_i.ProfileData);
        printf("info.ProfileSize     : %d\n", bmp_i.ProfileSize);
        printf("info.Reserved        : %d\n", bmp_i.Reserved);
}

const void logBMP(const BMP bmp) {
        logBMP_Header(bmp.header);
        logBMP_Info(bmp.info);

}


