#include "../../headers/components/opengl_config.h"

/* SETUP GL CONTEXT */
const void setupGL(void) {
    if ( (glc = glXCreateContext(displ, vinfo, NULL, GL_TRUE)) == 0 ) {
        printf("Unable to create gl context. setupGL() -- glxCreateContext().\n");
    }

    glXMakeCurrent(displ, mainwin, glc);
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.00, 0.00, 0.00, 1.00);

    /* Initialize Glew and check for Errors. */
    const GLenum err = glewInit();
    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        printf("Error: %s\n", glewGetErrorString(err));
    }
    printf("Status: Using GLEW  : %s\n", glewGetString(GLEW_VERSION));
    printf("Status: GL VERSION  : %s\n", glGetString(GL_VERSION));
    printf("Status: GLSL VERSION: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}
const void createBuffers(void) {
    /* Main Vertex Buffer Object buffer initiallization. */
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 32, (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 32, (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    /* Generate a framebuffer object to save the depth values for the shadow Map. */
    glGenFramebuffers(1, &shadowMapFBO);
    glGenFramebuffers(1, &mainFBO);
}
const void createTextures(void) {
    /* Main Textures. */
    glGenTextures(4, testTexture);
    printf("testTexture: %d, %d, %d, %d\n", testTexture[0], testTexture[1], testTexture[2], testTexture[3]);

    /* Create a 2D Texture to use it as the depth buffer for the Shadow Map. */
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    glGenTextures(1, &shadowDepthMap);
    printf("shadowDepthMap: %d\n", shadowDepthMap);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, shadowDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Attach the generated 2D Texture to our Shadow Map framebuffer's depth buffer */
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
        perror("Incomplete shadowMapFBO ");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* Create a 2D Texture to use it for clickSelect aka. (pick objects on mouse click). */
    glGenTextures(1, &mainColorMap);
    printf("mainColorMap: %d\n", mainColorMap);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, mainColorMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, main_wa.width, main_wa.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Create a user specific framebuffer to use it for rendering instead of the default framebuffer.*/
    glGenTextures(1, &mainDepthMap);
    printf("mainDepthMap: %d\n", mainDepthMap);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, mainDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, main_wa.width, main_wa.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Create a INFO texture.*/
    glGenTextures(1, &mainInfoMap);
    printf("mainInfoMap: %d\n", mainInfoMap);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, mainInfoMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32I, main_wa.width, main_wa.height, 0, GL_RG_INTEGER, GL_INT, NULL);

    /* Attach the generated 2D Texture to our Shadow Map framebuffer's depth buffer */
    glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mainColorMap, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mainDepthMap, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mainInfoMap, 0);
    if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
        perror("Incomplete mainFBO ");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


