#include "../../headers/shaders/shadowShader.h"

const static char *shadowVertexShaderSource = "#version 450 core\n"
    "layout (location = 0) in vec3 vsPos;\n"

    "uniform mat4 lightSpaceMatrix;\n"
    "uniform mat4 modelMatrix;\n"

    "layout (location = 0) out vec4 fsPos;\n"

    "void main() {\n"

    "    gl_Position = (lightSpaceMatrix * modelMatrix) * vec4(vsPos, 1.f);\n"
    // "    fsPos = gl_Position;\n"

    "}\n\0";
const static char *shadowFragmentShaderSource = "#version 450 core\n"
    // "layout (location = 0) in vec4 fsPos;\n"

    // "layout (location = 0) out vec4 FragColor;\n"

    // "float near = 0.1f;\n"
    // "float far = 100.f;\n"
    // "float linearizeDepth(float depth) {\n"
    // "    float z = (depth * 2.f) - 1.f;\n"
    // "    return (2.f * near * far) / (far + near - z * (far - near));\n"
    // "};\n"

    "void main() {\n"
    // "    gl_FragDepth = linearizeDepth(gl_FragCoord.z) / far;\n"
    // "    gl_FragDepth = gl_FragCoord.z;\n"
    "}\n\0";

const int initShadowShader(void) {
    int success, vertexShader, fragmentShader, shaderProgram;
    char infoLog[512];
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &shadowVertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED.\n%s\n", infoLog);
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &shadowFragmentShaderSource, NULL);
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
const void shadowCast(void) {

    glUseProgram(shadowShaderProgram);

    glViewport(0, 0, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    lookAtMatrix = lookat(scene.m[7].cd.v[P], scene.m[7].cd.v[U], scene.m[7].cd.v[V], scene.m[7].cd.v[N]);
    viewMatrix = inverse_mat(lookAtMatrix);
    lightMatrix = mxm(viewMatrix, orthoMatrix);

    GLfloat vpMatrix[16], modelMatrix[16];
    memcpy(&vpMatrix, &lightMatrix, 64);

    glUniformMatrix4fv(0, 1, GL_FALSE, vpMatrix);

    for (int i = 0; i < scene.m_indexes; i++) {

        Mat4x4 sclMatrix, trMatrix;

        vec4f pos = { 0 };
        Mat4x4 mfQ = MatfromQuat(scene.m[i].Q, pos);
        sclMatrix = mxm(mfQ, scaleMatrix(scene.m[i].scale));
        trMatrix = translationMatrix(scene.m[i].cd.v[P][0], scene.m[i].cd.v[P][1], scene.m[i].cd.v[P][2]);
        scene.m[i].modelMatrix = mxm(sclMatrix, trMatrix);
        memcpy(&modelMatrix, &scene.m[i].modelMatrix, 64);

        glUniformMatrix4fv(1, 1, GL_FALSE, modelMatrix);

        glBufferData(GL_ARRAY_BUFFER, scene.m[i].vba_indexes * 32, scene.m[i].vba, GL_STATIC_DRAW);

        glDrawArrays(GL_TRIANGLES, 0, scene.m[i].vba_indexes);
    }

    // GLenum err;
    // while ((err = glGetError()) != GL_NO_ERROR) {
    //     fprintf(stderr, "shadowCast < %d >  ", err);
    //     perror("OpenGL ERROR: ");
    // }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


