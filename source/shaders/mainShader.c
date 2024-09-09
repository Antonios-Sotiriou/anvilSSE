#include "../../headers/shaders/mainShader.h"

const static char *vertexShaderSource = "#version 450 core\n"
    "layout (location = 0) in vec3 vsPos;\n"
    "layout (location = 1) in vec2 vsTexels;\n"
    "layout (location = 2) in vec3 vsNormal;\n"

    "uniform mat4 vpMatrix;\n"
    "uniform mat4 lightSpaceMatrix;\n"
    "uniform mat4 modelMatrix;\n"

    "out VS_OUT {\n"
    "    vec4 fsPos;\n"
    "    vec3 fragPosWS;\n"
    "    vec2 fsTexels;\n"
    "    vec3 fsNormal;\n"
    "    vec4 fsPosLightSpace;\n"
    "} vs_out;\n"

    "void main() {\n"

    "    vs_out.fsPos = (vpMatrix * modelMatrix) * vec4(vsPos, 1.f);\n"
    "    vs_out.fragPosWS = vec3(modelMatrix * vec4(vsPos, 1.f));\n"
    "    vs_out.fsTexels = vsTexels;\n"
    "    vs_out.fsNormal = mat3(modelMatrix) * vsNormal;\n"
    "    vs_out.fsPosLightSpace = (lightSpaceMatrix * modelMatrix) * vec4(vsPos, 1.f);\n"

    "    gl_Position = vs_out.fsPos;\n"
    "}\n\0";
const static char *fragmentShaderSource = "#version 450 core\n"
    "in VS_OUT {\n"
    "    vec4 fsPos;\n"
    "    vec3 fragPosWS;\n"
    "    vec2 fsTexels;\n"
    "    vec3 fsNormal;\n"
    "    vec4 fsPosLightSpace;\n"
    "} fs_in;\n"

    "uniform vec3 lightPos;\n"
    "uniform vec3 cameraPos;\n"
    "uniform sampler2D shadowDepthMap;\n"
    "uniform sampler2D ourTexture[4];\n"
    "uniform int v_index;\n"

    "layout (location = 0) out vec4 FragColor;\n"

    "float shadowCalculation(vec4 fsPosLightSpace) {\n"
        // Perform Perspective Divide.
    "    vec3 projCoords = fsPosLightSpace.xyz / fsPosLightSpace.w;\n"
        // Transform to 0.f - 1.f range.
    "    projCoords = (projCoords * 0.5f) + 0.5f;\n"
        // Get closest depth value from light's perspective (using [0, 1] range fsPosLightSpace as coords).
    "    float closestDepth = texture(shadowDepthMap, projCoords.xy).r;\n"
        // Get depth of current fragment from light's perspective.
    "    float currentDepth = projCoords.z;\n"
        // Calculate bias (based on shadowDepthMap resolution and slope).
    "    vec3 normal = normalize(fs_in.fsNormal);\n"
    "    vec3 lightDir = normalize(lightPos - fs_in.fragPosWS);\n"
    "    float bias = max(0.05f * (1.f - dot(normal, lightDir)), 0.005f);\n"
        // Check whethercurrent fragment position is in shadow.
        // PCF.
    "    float shadow = 0.f;\n"
    "    vec2 texelSize = 1.f / textureSize(shadowDepthMap, 0);\n"
    "    for (int x = -1; x <= 1; x++) {\n"
    "        for (int y = -1; y <= 1; y++) {\n"
    "            float pcfDepth = texture(shadowDepthMap, projCoords.xy + vec2(x, y) * texelSize).r;\n"
    "            shadow += currentDepth - bias > pcfDepth ? 1.f : 0.f;\n"
    "        }\n"
    "    }\n"
    "    shadow /= 9.f;\n"
        // Keep the shadow at 0.f when outside the far_plane region of the light's frustum.
    "    if ( projCoords.z > 1.f )\n"
    "        shadow = 0.f;\n"

    "    return shadow;\n"
    "}\n"

    "void main() {\n"

    "    float ambientStr = 0.1;\n"
    "    vec3 ambient = ambientStr * vec3(1.f, 1.f, 1.f);\n"

    "    vec3 norm = normalize(fs_in.fsNormal);\n"
    "    vec3 lightDir = normalize(lightPos - fs_in.fragPosWS);\n"
    "    float diff = max(dot(norm, lightDir), 0.f);\n"
    "    vec3 diffuse = diff * vec3(1.f, 1.f, 1.f);\n"

    "    float specularStr = 0.5;\n"
    "    vec3 viewDir = normalize(cameraPos - fs_in.fragPosWS);\n"

    // "    vec3 reflectDir = normalize(lightDir + viewDir);\n"            // Blinn-Phong
    "    vec3 reflectDir = reflect(-lightDir, norm);\n"              // Phong

    // "    float spec = pow(max(dot(norm, reflectDir), 0.f), 32);\n"        // Blinn-Phong
    "    float spec = pow(max(dot(viewDir, reflectDir), 0.f), 32);\n"  // Phong
    "    vec3 specular = specularStr * spec * vec3(1.f, 1.f, 1.f);\n"

    "    float shadow = shadowCalculation(fs_in.fsPosLightSpace);\n"
    "    vec3 result = ((ambient + (1.f - shadow)) * (diffuse + specular)) * texture(ourTexture[v_index], fs_in.fsTexels).bgr;\n"

    "    FragColor = vec4(result, 1.f);\n"
    "}\n\0";

const int initMainShader(void) {
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


