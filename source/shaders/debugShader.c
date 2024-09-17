#include "../../headers/shaders/debugShader.h"

const static char *debugVertexShaderSource = "#version 450 core\n"
    "layout (location = 0) in vec3 vsPos;\n"
    "layout (location = 1) in vec2 vsTexels;\n"

    "layout (location = 0) out vec2 fsTexels;\n"

    "void main() {\n"

    "    gl_Position = vec4(vsPos, 1.f);\n"
    "    fsTexels = vsTexels;\n"

    "}\n\0";
const static char *debugFragmentShaderSource = "#version 450 core\n"
    "layout (location = 0) in vec2 fsTexels;\n"

    "uniform sampler2D debugTexture;\n"

    "layout (location = 0) out vec4 FragColor;\n"

    "void main() {\n"
    "    float depthValue = texture(debugTexture, fsTexels).r;\n"
    "    FragColor = vec4(vec3(depthValue), 1.f);\n"

    "}\n\0";

const int initDebugShader(void) {
    int success, vertexShader, fragmentShader, shaderProgram;
    char infoLog[512];
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &debugVertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED.\n%s\n", infoLog);
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &debugFragmentShaderSource, NULL);
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
/* Displays texture with given texture index on full screen. */
const void displayTexture(const int textureIndex) {

    glUseProgram(debugShaderProgram);

    glViewport(0, 0, main_wa.width, main_wa.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniform1i(0, textureIndex);

    vec8f quad[4] = {
        { 1.f, -1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f },
        { -1.f, -1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f },
        { 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f },
        { -1.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f }
    };

    for (int i = 0; i < 4; i++) {

        glBufferData(GL_ARRAY_BUFFER, 4 * 32, quad, GL_STATIC_DRAW);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "displayTexture < %d >  ", err);
        perror("OpenGL ERROR: ");
    }

    glXSwapBuffers(displ, mainwin);
}


