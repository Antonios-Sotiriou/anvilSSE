#include "../../headers/shaders/debugShader.h"

const static char *debugVertexShaderSource = "#version 450 core\n"
    "layout (location = 0) in vec3 vsPos;\n"
    "layout (location = 1) in vec2 vsTexels;\n"

    "layout (location = 0) out vec4 fsPos;\n"
    "layout (location = 1) out vec2 fsTexels;\n"

    "void main() {\n"

    "    gl_Position = vec4(vsPos, 1.f);\n"
    "    fsPos = gl_Position;\n"
    "    fsTexels = vsTexels;\n"

    "}\n\0";
const static char *debugFragmentShaderSource = "#version 450 core\n"
    "layout (location = 0) in vec4 fsPos;\n"
    "layout (location = 1) in vec2 fsTexels;\n"

    "uniform sampler2D shadowDepthMap;\n"

    "layout (location = 0) out vec4 FragColor;\n"

    "float near = 0.1f;\n"
    "float far = 1000.f;\n"

    "float linearizeDepth(float depth) {\n"
    "    float z = (depth * 2.f) - 1.f;\n"
    "    return (2.f * near * far) / (far + near - z * (far - near));\n"
    "};\n"

    "void main() {\n"

    // "    float depthValue = linearizeDepth(gl_FragCoord.z) / far;\n"
    // "    FragColor = vec4(vec3(depthValue), 1.f);\n"
    "    float depthValue = texture(shadowDepthMap, fsTexels).r;\n"
    // "    FragColor = vec4(vec3(linearizeDepth(depthValue) / far), 1.f);\n"
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

