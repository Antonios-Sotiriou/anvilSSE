#include "../../headers/shaders/clickSelectShader.h"

const static char *vertexShaderSource = "#version 450 core\n"
    "layout (location = 0) in vec3 vsPos;\n"

    "uniform mat4 vpMatrix;\n"
    "uniform mat4 mMatrix;\n"
    // "uniform int mesh_id;\n"

    "layout (location = 0) out vec4 fsPos;\n"
    // "layout (location = 1) out int id;\n"

    "void main() {\n"
    // "    id = mesh_id;\n"
    "    fsPos = mMatrix * vec4(vsPos, 1.f);\n"
    "    gl_Position = vpMatrix * fsPos;\n"
    "}\n\0";
const static char *fragmentShaderSource = "#version 450 core\n"
    "layout (location = 0) in vec4 fsPos;\n"
    // "layout (location = 1) flat in int id;\n"

    "layout (location = 0) out vec4 FragColor;\n"

    "float near = 0.1f;\n"
    "float far = 100.f;\n"
    "float linearizeDepth(float depth) {\n"
    "    float z = (depth * 2.f) - 1.f;\n"
    "    return (2.f * near * far) / (far + near - z * (far - near));\n"
    "};\n"

    "void main() {\n"

    "    FragColor = gl_FragCoord;\n"

    "    float depth = linearizeDepth(gl_FragCoord.z) / far;\n"
    // "    FragColor = vec4(depth, depth, depth, 1.f);\n"
    "    gl_FragDepth = depth;\n"
    "}\n\0";

const int initClickSelectShader(void) {
    int success, vertexShader, fragmentShader, shaderProgram;
    char infoLog[512];
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED.\n%s\n", infoLog);
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED.\n%s\n", infoLog);
    }

    /* Shaders programm creation and linking. */
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::PROGRAMM::LINKING_FAILED.\n%s\n", infoLog);
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}
// const int clickSelect() {

//     glUseProgram(clickSelectShaderProgram);

//     glViewport(0, 0, main_wa.width, main_wa.height);
//     glBindFramebuffer(GL_FRAMEBUFFER, clickSelectMapFBO);
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     GLfloat vpMatrix[16], mMatrix[16];
//     memcpy(&vpMatrix, &worldMatrix, 64);

//     glUniformMatrix4fv(0, 1, GL_FALSE, vpMatrix);

//     for (int i = 0; i < scene.m_indexes; i++) {

//         if ( frustumCulling(scene.m[i].bbox.v, scene.m[i].bbox.v_indexes, worldMatrix) ) {

//             Mat4x4 sclMatrix, trMatrix;

//             vec4f pos = { 0 };
//             Mat4x4 mfQ = MatfromQuat(scene.m[i].Q, pos);
//             sclMatrix = mxm(mfQ, scaleMatrix(scene.m[i].scale));
//             trMatrix = translationMatrix(scene.m[i].cd.v[P][0], scene.m[i].cd.v[P][1], scene.m[i].cd.v[P][2]);
//             scene.m[i].modelMatrix = mxm(sclMatrix, trMatrix);
//             memcpy(&mMatrix, &scene.m[i].modelMatrix, 64);

//             glUniformMatrix4fv(1, 1, GL_FALSE, mMatrix);
//             // glUniform1i(2, scene.m[i].id);

//             glBufferData(GL_ARRAY_BUFFER, scene.m[i].vba_indexes * 32, scene.m[i].vba, GL_STATIC_DRAW);

//             glDrawArrays(GL_TRIANGLES, 0, scene.m[i].vba_indexes);
//         }
//     }

//     GLenum err;
//     while ((err = glGetError()) != GL_NO_ERROR) {
//         fprintf(stderr, "clickSelect < %d >  ", err);
//         perror("OpenGL ERROR: ");
//     }

//     float pixel[4] = { 0 };
//     glReadPixels(click[0], main_wa.height - click[1], 1, 1, GL_RGBA, GL_FLOAT, &pixel);
//     vec4f r = { pixel[0], pixel[1], pixel[2], pixel[3] };

//     const int id = pointScreenToWorld(r);

//     glBindFramebuffer(GL_FRAMEBUFFER, 0);

//     return id;
// }
// const int pointScreenToWorld(const vec4f cs) {
//     float w = 1 / cs[3];
//     vec4f wsc = {
//         ((cs[0] / (main_wa.width >> 1)) - 1.0) * w,
//         ((cs[1] / (main_wa.height >> 1)) - 1.0) * w,
//         (1.f / cs[2]) * w,
//         w
//     };

//     wsc = vecxm(wsc, reperspMatrix);
//     wsc[3] = 1.f;
//     wsc = vecxm(wsc, lookAtMatrix);

//     const int id = pointVsBbox(&Gitana, &scene, wsc);

//     return id;
// }
// /* Retrieves object's id from terrain Quad index, with which world space ( wsc ) vector collides */
// const int pointVsBbox(TerrainInfo *ti, Scene *s, vec4f wsc) {
//     const int quadIndex = getTerrainQuadIndex(&s->m[Terrain_1], wsc);

//     if (quadIndex < 0) {
//         fprintf(stderr, "obj->quadIndex : %d. Out of Terrain. ObjectEnvironmentCollision().\n", quadIndex);
//         return -1;
//     }
//     vec4i iwsc = __builtin_convertvector(wsc + 0.5f, vec4i);

//     Mesh *cache;
//     const int num_of_members = ti->quads[quadIndex].members_indexes;
//     for (int i = 0; i < num_of_members; i++) {

//         cache = ti->quads[quadIndex].members[i];

//         cache->dm = getDimensionsLimits(cache->bbox.v, cache->bbox.v_indexes);
//         vec4i min = __builtin_convertvector(cache->dm.min + 0.5, vec4i);
//         vec4i max = __builtin_convertvector(cache->dm.max + 0.5f, vec4i);

//         if ( iwsc[2] >= min[2] && iwsc[2] <= max[2] ) {

//             if ( iwsc[0] >= min[0] && iwsc[0] <= max[0] ) {

//                 if ( iwsc[1] >= min[1] && iwsc[1] <= max[1] ) {

//                     return cache->id;
//                 }

//             }
//         }
//     }

//     logTerrainQuad(*ti, quadIndex);
//     return -1;
// }


