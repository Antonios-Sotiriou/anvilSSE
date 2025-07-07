#include "../../headers/shaders/mainShader.h"

const static int frustumCulling(vec4f v[], const int v_indexes, const Mat4x4 wm);

const static char *vertexShaderSource = "#version 450 core\n"
    "layout (location = 0) in vec3 vsPos;\n"
    "layout (location = 1) in vec2 vsTexels;\n"
    "layout (location = 2) in vec3 vsNormal;\n"

    "uniform mat4 vpMatrix;\n"
    "uniform mat4 lightSpaceMatrix;\n"
    "uniform mat4 modelMatrix;\n"
    "uniform int mesh_id;\n"

    "out VS_OUT {\n"
    "    vec4 fsPos;\n"
    "    vec3 fragPosWS;\n"
    "    vec2 fsTexels;\n"
    "    vec3 fsNormal;\n"
    "    vec4 fsPosLightSpace;\n"
    "    int id;\n"
    "} vs_out;\n"

    "void main() {\n"

    "    vs_out.fsPos = (vpMatrix * modelMatrix) * vec4(vsPos, 1.f);\n"
    "    vs_out.fragPosWS = vec3(modelMatrix * vec4(vsPos, 1.f));\n"
    "    vs_out.fsTexels = vsTexels;\n"
    "    vs_out.fsNormal = mat3(modelMatrix) * vsNormal;\n"
    "    vs_out.fsPosLightSpace = (lightSpaceMatrix * modelMatrix) * vec4(vsPos, 1.f);\n"

    "    gl_Position = vs_out.fsPos;\n"
    "    vs_out.id = mesh_id;\n"
    "}\n\0";
const static char *fragmentShaderSource = "#version 450 core\n"
    "in VS_OUT {\n"
    "    vec4 fsPos;\n"
    "    vec3 fragPosWS;\n"
    "    vec2 fsTexels;\n"
    "    vec3 fsNormal;\n"
    "    vec4 fsPosLightSpace;\n"
    "    flat int id;\n"
    "} fs_in;\n"

    "uniform vec3 lightPos;\n"
    "uniform vec3 cameraPos;\n"
    "uniform sampler2D shadowDepthMap;\n"
    "uniform sampler2D tex_index;\n"

    "layout (location = 0) out vec4 FragColor;\n"
    "layout (location = 1) out ivec2 mesh_info;\n"

    // "float near = 0.1f;\n"
    // "float far = 100.f;\n"
    // "float linearizeDepth(float depth) {\n"
    // "    float z = (depth * 2.f) - 1.f;\n"
    // "    return (2.f * near * far) / (far + near - z * (far - near));\n"
    // "};\n"

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
    "    vec3 result = ((ambient + (1.f - shadow)) * (diffuse + specular)) * texture(tex_index, fs_in.fsTexels).bgr;\n"

    "    FragColor = vec4(result, 1.f);\n"
    "    mesh_info = ivec2(fs_in.id, gl_PrimitiveID);\n"
    // "    gl_FragDepth = linearizeDepth(gl_FragCoord.z) / far;\n"
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
/* Cull Mesh to view frustum. viewProj: (1 for Prespective and 0 for orthographic Projection). Thats for the perspective divide usefull.viewMatrix the matrix of the point of view. */
const static int frustumCulling(vec4f v[], const int v_indexes, const Mat4x4 wm) {
    vec4f *vec_arr = vecsarrayxm(v, v_indexes, wm);

    for (int j = 0; j < v_indexes; j++) {
        vec_arr[j] /= vec_arr[j][3];
    }

    DimensionsLimits dm = getDimensionsLimits(vec_arr, v_indexes);

    if ( ((dm.min[2] > 1.f) || (dm.max[2] < -1.f)) ||
         ((dm.min[1] > 1.f) || (dm.max[1] < -1.f)) ||
         ((dm.min[0] > 1.f) || (dm.max[0] < -1.f)) ) {

        free(vec_arr);
        return 0;
    }

    free(vec_arr);
    return 1;
}
const void project(void) {

    glUseProgram(mainShaderProgram);

    glViewport(0, 0, 1000, 1000);
    glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
    glDrawBuffers(2, drawBuffers);
    glClear(GL_DEPTH_BUFFER_BIT);

    lookAtMatrix = lookat(scene.m[EYEPOINT].cd.v[P], scene.m[EYEPOINT].cd.v[U], scene.m[EYEPOINT].cd.v[V], scene.m[EYEPOINT].cd.v[N]);
    viewMatrix = inverse_mat(lookAtMatrix);
    worldMatrix = mxm(viewMatrix, perspMatrix);

    GLfloat vpMatrix[16], lightSpaceMatrix[16], modelMatrix[16], lightPos[3], cameraPos[3];
    memcpy(&vpMatrix, &worldMatrix, 64);
    memcpy(&lightSpaceMatrix, &lightMatrix, 64);
    memcpy(&lightPos, &scene.m[7].cd.v[P], 12);
    memcpy(&cameraPos, &scene.m[6].cd.v[P], 12);

    glUniformMatrix4fv(0, 1, GL_FALSE, vpMatrix);
    glUniformMatrix4fv(1, 1, GL_FALSE, lightSpaceMatrix);
    glUniform3fv(4, 1, lightPos);
    glUniform3fv(5, 1, cameraPos);
    glUniform1i(6, 4);

    if (!DEBUG)
        glPolygonMode(GL_FRONT, GL_FILL);
    else if (DEBUG == 1)
        glPolygonMode(GL_FRONT, GL_LINE);
    else
        glPolygonMode(GL_FRONT, GL_POINT);

    for (int i = 0; i < scene.m_indexes; i++) {

        const int distance = len_vec(scene.m[i].cd.v[P] - scene.m[6].cd.v[P]);
        adoptdetailMesh(&scene.m[i], distance);
        // adoptdetailTexture(&scene.m[i], distance);

        if ( frustumCulling(scene.m[i].bbox.v, scene.m[i].bbox.v_indexes, worldMatrix) ) {

            Mat4x4 sclMatrix, trMatrix;

            vec4f pos = { 0 };
            Mat4x4 mfQ = MatfromQuat(scene.m[i].Q, pos);
            sclMatrix = mxm(mfQ, scaleMatrix(scene.m[i].scale));
            trMatrix = translationMatrix(scene.m[i].cd.v[P][0], scene.m[i].cd.v[P][1], scene.m[i].cd.v[P][2]);
            scene.m[i].modelMatrix = mxm(sclMatrix, trMatrix);
            memcpy(&modelMatrix, &scene.m[i].modelMatrix, 64);

            glUniformMatrix4fv(2, 1, GL_FALSE, modelMatrix);

            glUniform1i(7, scene.m[i].tex_index);
            glUniform1i(3, scene.m[i].id);

            glBufferData(GL_ARRAY_BUFFER, scene.m[i].vba_indexes * 32, scene.m[i].vba, GL_STATIC_DRAW);

            glDrawArrays(GL_TRIANGLES, 0, scene.m[i].vba_indexes);
        }
    }

    // GLenum err;
    // while ( (err = glGetError()) != GL_NO_ERROR ) {
    //     fprintf(stderr, "project < %d >  ", err);
    //     perror("OpenGL ERROR: ");
    // }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // displayPoint(scene.m[1].cd.v[P], worldMatrix, 0xff00a7); //0028ff
    if (COLLISION) {
        face f = facexm(scene.m[COLLIDINGMESH].f[COLLIDINGFACE], scene.m[COLLIDINGMESH].modelMatrix);
        // displayFace(&f, worldMatrix); //0028ff
        displayFilledFace(&f, worldMatrix);
    }
    if (DISPLAYBBOX) {
        displayMeshKinetics(&scene.m[mesh_id], worldMatrix); //0028ff
        displayBboxFaces(&scene.m[mesh_id], worldMatrix); //0028ff
    }
}


