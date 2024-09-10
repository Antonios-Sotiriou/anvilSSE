#include "../../headers/shaders/clickSelectShader.h"

const static char *vertexShaderSource = "#version 450 core\n"
    "layout (location = 0) in vec3 vsPos;\n"

    "uniform mat4 vpMatrix;\n"
    "uniform mat4 modelMatrix;\n"

    "layout (location = 0) out vec4 fsPos;\n"

    "void main() {\n"

    "    fsPos = (vpMatrix * modelMatrix) * vec4(vsPos, 1.f);\n"
    "    gl_Position = fsPos;\n"
    "}\n\0";
const static char *fragmentShaderSource = "#version 450 core\n"
    "layout (location = 0) in vec4 fsPos;\n"

    "layout (location = 0) out vec4 FragColor;\n"

    "float near = 0.1f;\n"
    "float far = 100.f;\n"
    "float linearizeDepth(float depth) {\n"
    "    float z = (depth * 2.f) - 1.f;\n"
    "    return (2.f * near * far) / (far + near - z * (far - near));\n"
    "};\n"

    "void main() {\n"

    "    FragColor = vec4(linearizeDepth(gl_FragCoord.z) / far);\n"
    // "    FragColor = fsPos;\n"
    // "    gl_FragDepth = fsPos.z;\n"
    // "    gl_FragDepth = linearizeDepth(gl_FragCoord.z) / far;\n"
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