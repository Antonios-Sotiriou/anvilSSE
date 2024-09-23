/* General Macro definitions */
#define _GNU_SOURCE /* Importand to counter sigaction struct < incomplete type error >. */

/* general headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <sys/time.h>

/* OpenGL headers. */
#include <GL/glew.h>     /* libglew-dev */
#include <GL/glx.h>     /* libglx-dev */
#include <GL/glu.h>     /* libglu1-mesa-dev */

int mainShaderProgram, shadowShaderProgram, debugShaderProgram, clickSelectShaderProgram;
/* Locations of uniforms of the shaders. */
GLint transformLocA, transformLocB, transformLocC, transformLocD, transformLocE, transformLocF, transformLocG, transformLocH, transformLocI, texLoc0, texLoc1, texLoc2, texLoc3;

/* Library to cooperate with Postgres Database. */
#include <postgresql/libpq-fe.h>  /* libpq-dev */

/* signal */
#include <signal.h>
// #include <immintrin.h>

/* testing */
#include "headers/components/exec_time.h"
#include "headers/components/logging.h"
#include "headers/components/test_shapes.h"

/* ############################################## MULTITHREADING ################################################################### */
#include <pthread.h>
pthread_t threads[THREADS];
int *thread_ids;
/* ############################################## MULTITHREADING ################################################################### */

/* Variable to enable the reach of a gdb debugger breakpoint. */
int ENABLE_GDB_BREAKPOINT = 0;

/* Project specific headers */
#include "headers/shaders/mainShader.h"
#include "headers/shaders/shadowShader.h"
#include "headers/shaders/debugShader.h"
#include "headers/shaders/clickSelectShader.h"
#include "headers/components/anvil_structs.h"
#include "headers/components/quaternions.h"
#include "headers/components/database.h"
#include "headers/components/matrices.h"
#include "headers/components/kinetics.h"
#include "headers/components/scene_objects.h"
#include "headers/components/general_functions.h"
#include "headers/components/physics.h"
#include "headers/components/terrain_functions.h"
#include "headers/components/collision_detection.h"
#include "headers/components/camera.h"

enum { Win_Close, Win_Name, Atom_Type, Atom_Last};

#define WIDTH                     1000
#define HEIGHT                    1000
#define POINTERMASKS              ( ButtonPressMask ) // PointerMotionMask
#define KEYBOARDMASKS             ( KeyPressMask | KeyReleaseMask )
#define EXPOSEMASKS               ( StructureNotifyMask )

/* X Global Structures. */
Display *displ;
Window root, mainwin;
GC gc;
XGCValues gcvalues;
XWindowAttributes main_wa, *point_attrib;
XSetWindowAttributes sa;
Atom wmatom[Atom_Last];

/* OpenGl Global variables. */
GLXContext              glc;
Colormap                cmap;
XVisualInfo             *vinfo;
GLint                   att[]   = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
GLint VBO, testTexture[4], shadowDepthMap, shadowMapFBO, mainColorMap, mainDepthMap, mainInfoMap, mainFBO;
GLenum drawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

/* Project Global Variables. */
int PROJECTIONVIEW        = 0;
static int PROJECTBUFFER  = 1;
static int EYEPOINT       = 0;
static float FOV          = 45.0f;
static float ZNEAR        = -10.f;
static float ZFAR         = -__INT32_MAX__;
float ASPECTRATIO         = 1;
static int BSIZE          = 0;
float AmbientStrength     = 0.5f;
float SpecularStrength    = 0.5f;
float DiffuseStrength     = 0.5f;
float shadow_bias         = 0.0f;//0.003105;//0.002138;//0.000487f;
int DISPLAYBBOX           = 0;
/* For investigating shadow map usefull global variables. */
int INCORDEC = -1;
int SMA = 0;
int SMB = 2000;
int SMC = 4000;
int SMD = 8000;
int STA = 820;
int STB = 3200;
int STC = 12800;
int STD = 51200;

/* Point of view. */
Mesh *eye;

vec4f gravity_epicenter = { 0.f, -1.f, 0.f };
const float sunMov = 100.0f;
const float movScalar = 1.f;
const float moveForce = 0.2f;

/* Variables usefull for mesh click select. */
int getClick = 0;
int mesh_id = 0, primitive_id = 0;
vec4f click = { 0 };

/* Global Matrices */
Mat4x4 perspMatrix, lookAtMatrix, viewMatrix, reperspMatrix, orthoMatrix, worldMatrix, lightMatrix, ortholightMat[NUM_OF_CASCADES], *point_mat; 

/* Anvil global Objects Meshes and Scene. */
Scene scene = { 0 };
/* Terrain info struct is populated with data when terrain is created(createTerrain()). */
TerrainInfo Gitana;

/* X11 and mainwindow Global variables. */
static int INIT = 0;
static int RUNNING = 1;
vec4i half_screen = { 0, 0, 1, 1 }; // This variable is initialized in configurenotify function.Its Helping us decrease the number of divisions.
int DEBUG = 0;

/* Display usefull measurements. */
float			        TimeCounter = 0, LastFrameTimeCounter = 0, DeltaTime = 0, prevTime = 0.0, FPS = 0;
struct timeval		    tv, tv0;
int			            Frame = 0;

/* Event handling functions. */
const static void clientmessage(XEvent *event);
const static void reparentnotify(XEvent *event);
const static void mapnotify(XEvent *event);
const static void resizerequest(XEvent *event);
const static void configurenotify(XEvent *event);
const static void buttonpress(XEvent *event);
const static void keypress(XEvent *event);
const static void keyrelease(XEvent *event);
const static void mousemotion(XEvent *event);

/* Representation functions */
const int frustumCulling(vec4f v[], const int v_indexes, const Mat4x4 wm);

/* Xlib relative functions and event dispatcher. */
const static void initMainWindow(void);
const static void setupGL(void);
const void createBuffers(void);
const void createTextures(void);
const int assignUniformLocations(void);
const static void initGlobalGC(void);
const static void initDependedVariables(void);
const static void initAtoms(void);
// const static void initLightModel(Mesh *m);
const static void InitTimeCounter(void);
const static void UpdateTimeCounter(void);
const static void CalculateFPS(void);
const static void displayInfo(void);
const static void sigsegv_handler(const int sig);
const static int registerSig(const int signal);
const static int board(void);
static void (*handler[LASTEvent]) (XEvent *event) = {
    [ClientMessage] = clientmessage,
    [ReparentNotify] = reparentnotify,
    [MapNotify] = mapnotify,
    [ResizeRequest] = resizerequest,
    [ConfigureNotify] = configurenotify,
    [ButtonPress] = buttonpress,
    [KeyPress] = keypress,
    [KeyRelease] = keyrelease,
    [MotionNotify] = mousemotion,
};

const static void clientmessage(XEvent *event) {
    printf("Received client message event\n");
    if (event->xclient.data.l[0] == wmatom[Win_Close]) {

        for (int i = 0; i < Gitana.quadsArea; i++) {
            if (Gitana.quads[i].members)
                free(Gitana.quads[i].members);
        }
        free(Gitana.quads);

        releaseScene(&scene);

        XFreeGC(displ, gc);

        // 4. Unbinding the Vertex and Buffer arrays.
        XFree(vinfo);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &shadowDepthMap);
        glDeleteBuffers(1, &shadowMapFBO);
        glDeleteBuffers(1, &mainColorMap);
        glDeleteBuffers(1, &mainDepthMap);
        glDeleteBuffers(1, &mainFBO);

        glDeleteProgram(mainShaderProgram);
        glDeleteProgram(shadowShaderProgram);
        glDeleteProgram(debugShaderProgram);
        glDeleteProgram(clickSelectShaderProgram);

        glXMakeCurrent(displ, None, NULL);
        glXDestroyContext(displ, glc);

        XDestroyWindow(displ, mainwin);

        RUNNING = 0;
    }
}
const static void reparentnotify(XEvent *event) {

    // printf("reparentnotify event received\n");
}
const static void mapnotify(XEvent *event) {

    // printf("mapnotify event received\n");
}
const static void resizerequest(XEvent *event) {

    // printf("resizerequest event received\n");
}
const static void configurenotify(XEvent *event) {
    // printf("configurenotify event received\n");
    if (!event->xconfigure.send_event) {
        // printf("configurenotify Send event received\n");
        XGetWindowAttributes(displ, mainwin, &main_wa);

        if (INIT) {

            initDependedVariables();

        } else {
            INIT = 1;
        }
    }
}
const static void buttonpress(XEvent *event) {
    system("clear\n");
    printf("buttonpress event received\n");
    // printf("X: %f\n", ((event->xbutton.x - (WIDTH / 2.00)) / (WIDTH / 2.00)));
    // printf("Y: %f\n", ((event->xbutton.y - (HEIGHT / 2.00)) / (HEIGHT / 2.00)));
    click[0] = event->xbutton.x;
    click[1] = event->xbutton.y;
    getClick = 1;
    // mesh_id = clickSelect();
    // printf("camera Position   :");
    // logDm(scene.m[6].dm);
}
const static void mousemotion(XEvent *event) {
    printf("MotionNotify event received\n");
}
const static void keypress(XEvent *event) {

    KeySym keysym = XLookupKeysym(&event->xkey, 0);

    // printf("\x1b[H\x1b[J");
    // system("clear\n");
    // printf("Key Pressed: %ld\n", keysym);
    // logEvent(*event);

    switch (keysym) {
        case 65505 : INCORDEC = INCORDEC == -1 ? 1 : -1; break; /* cntrl */
        case 103 : ENABLE_GDB_BREAKPOINT = ENABLE_GDB_BREAKPOINT == 0 ? 1 : 0; break; /* g */
        case 49 : SMA += INCORDEC; break;
        case 50 : SMB += INCORDEC; break;
        case 51 : SMC += INCORDEC; break;
        case 52 : SMD += INCORDEC; break;
        case 48 : STA += INCORDEC; break;
        case 57 : STB += INCORDEC; break;
        case 56 : STC += INCORDEC; break;
        case 55 : STD += INCORDEC; break;
        case 98 : DISPLAYBBOX = DISPLAYBBOX == 0 ? 1 : 0; break;  /* b */
        case 97 : look_left(&scene.m[6], 0.3);             /* a */
            break;
        case 100 : look_right(&scene.m[6], 0.3);           /* d */
            break;
        case 113 : look_up(&scene.m[6], 0.3);              /* q */
            break;
        case 101 : look_down(&scene.m[6], 0.3);            /* e */
            break;
        case 119 : move_forward(&scene.m[6], moveForce);         /* w */
            break;
        case 115 : move_backward(&scene.m[6], moveForce);        /* s */
            break;
        case 65361 : move_left(&scene.m[6], moveForce);          /* left arrow */
            break;
        case 65363 : move_right(&scene.m[6], moveForce);         /* right arrow */
            break;
        case 65362 : move_up(&scene.m[6], moveForce);            /* up arror */
            break;
        case 65364 : move_down(&scene.m[6], moveForce);          /* down arrow */
            break;
        case 32 :                                                /* Space */
            scene.m[6].grounded = 0;
            vec4f jump = { 0.f, 1.f, 0.f };
            scene.m[6].mvdir = jump;
            scene.m[6].momentum = 0.981 * scene.m[6].mass;
            scene.m[6].falling_time = 0.f;
            break;
        case 65451 :shadow_bias += 0.001;             /* + */
            printf("shadow_bias: %f\n", shadow_bias);
            break;
        case 65453 :shadow_bias -= 0.001;             /* - */
            printf("shadow_bias: %f\n", shadow_bias);
            break;
        case 65450 : SpecularStrength += 0.01f;             /* * */
            printf("SpecularStrength: %f\n", SpecularStrength);
            break;
        case 65455 : SpecularStrength -= 0.01f;             /* / */
            printf("SpecularStrength: %f\n", SpecularStrength);
            break;
        case 65430 : //sunlight.pos[0] -= sunMov;                   /* Adjust Light Source */
            // vec4f mva = norm_vec(camera[N] - camera[U]);
            vec4f mva = { -1.f, 0.f, 0.f };
            scene.m[1].mvdir = mva;
            scene.m[1].momentum = movScalar * scene.m[1].mass;
            scene.m[1].roll = 1;
            break;
        case 65432 : //sunlight.pos[0] += sunMov;                   /* Adjust Light Source */
            // vec4f mvb = -norm_vec(camera[N] - camera[U]);
            // vec4f mvb = norm_vec(scene.m[Camera_1].cd.v[U] + scene.m[Camera_1].cd.v[V]);
            vec4f mvb = { 1.f, 0.f, 0.f };
            scene.m[1].mvdir = mvb;
            scene.m[1].momentum = movScalar * scene.m[1].mass;
            scene.m[1].roll = 1;
            break;
        case 65434 : //sunlight.pos[1] += sunMov;                   /* Adjust Light Source */
            scene.m[1].grounded = 0;
            vec4f mvc = { 0.f, 1.f, 0.f };
            scene.m[1].mvdir = mvc;
            scene.m[1].momentum = 0.981 * scene.m[1].mass;
            scene.m[1].falling_time = 0.f;
            break;
        case 65435 : //sunlight.pos[1] -= sunMov;                   /* Adjust Light Source */
            vec4f mvd = { 0.f, -1.f, 0.f };
            scene.m[1].mvdir = mvd;
            scene.m[1].momentum = movScalar * scene.m[1].mass;
            break;
        case 65431 : //sunlight.pos[2] += sunMov;                   /* Adjust Light Source */
            // vec4f mve = norm_vec(scene.m[Camera_1].cd.v[U] + scene.m[Camera_1].cd.v[N]);
            vec4f mve = { 0.f, 0.f, 1.f };
            scene.m[1].mvdir = mve;
            scene.m[1].momentum = movScalar * scene.m[1].mass;
            scene.m[1].roll = 1;
            break;
        case 65433 : //sunlight.pos[2] -= sunMov;                   /* Adjust Light Source */
            // vec4f mvf = -norm_vec(scene.m[Camera_1].cd.v[U] + scene.m[Camera_1].cd.v[N]);
            vec4f mvf = { 0.f, 0.f, -1.f };
            scene.m[1].mvdir = mvf;
            scene.m[1].momentum = movScalar * scene.m[1].mass;
            scene.m[1].roll = 1;
            break;
        case 120 : rotateGlobalX(&scene.m[1], 1);                     /* x */
            break;
        case 121 : rotateGlobalY(&scene.m[1], 1);                    /* y */
            break;
        case 122 : rotateGlobalZ(&scene.m[1], 1);                     /* z */
            break;
        case 114 :    /* r */
            break;
        case 99 :                                                        /* c */
            if ( mesh_id > 0 )
                rotateMesh(&scene.m[mesh_id], 10, 0.0f, 1.0f, 0.0f);
            break;
        case 43 : AmbientStrength += 0.01;                                    /* + */
            printf("AmbientStrength: %f\n", AmbientStrength);
            break;
        case 45 : AmbientStrength -= 0.01;                                   /* - */
            printf("AmbientStrength: %f\n", AmbientStrength);
            break;
        case 112 :
            if (PROJECTBUFFER == 6)
                PROJECTBUFFER = 0;
            PROJECTBUFFER++;
            if (PROJECTBUFFER == 1) {
                fprintf(stderr, "Projecting Pixels -- PROJECTBUFFER: %d\n", PROJECTBUFFER);
            } else if (PROJECTBUFFER == 2) {
                fprintf(stderr, "Projecting Depth buffer -- PROJECTBUFFER: %d\n", PROJECTBUFFER);
            } else if (PROJECTBUFFER == 3) {
                fprintf(stderr, "Projecting Shadow buffer -- PROJECTBUFFER: %d\n", PROJECTBUFFER);
            }
            break;
        case 65507 :
            if (DEBUG == 2)
                DEBUG = -1;
            DEBUG++;
            if (DEBUG == 1) {
                fprintf(stderr, "Debug level 1 Enabled!: %d\n", DEBUG);
            } else if (DEBUG == 2) {
                fprintf(stderr, "Debug level 2 Enabled!: %d\n", DEBUG);
            }
            break;
        case 108 :                                    /* l */
            if (EYEPOINT == 0)
                EYEPOINT = 1;
            else
                EYEPOINT = 0;
            return;
        case 118 :
            if (!PROJECTIONVIEW)               /* v */
                PROJECTIONVIEW++;
            else
                PROJECTIONVIEW = 0;
            break;
    }
}
const static void keyrelease(XEvent *event) {

    KeySym keysym = XLookupKeysym(&event->xkey, 0);

    // printf("Key Released: %ld\n", keysym);
    if (eye) {
        eye->momentum = 0;
        eye->rot_angle = 0;
    }
    if ( keysym == 99 )
        scene.m[mesh_id].rot_angle = 0;
}
/* Cull Mesh to view frustum. viewProj: (1 for Prespective and 0 for orthographic Projection). Thats for the perspective divide usefull.viewMatrix the matrix of the point of view. */
const int frustumCulling(vec4f v[], const int v_indexes, const Mat4x4 wm) {
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
const void clickSelect() {

    glUseProgram(clickSelectShaderProgram);

    glViewport(0, 0, WIDTH, HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
    glStencilFunc(GL_ALWAYS, 1, 0XFF);
    glStencilMask(0xFF);
    glDrawBuffers(2, drawBuffers);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (EYEPOINT)
        eye = &scene.m[7];
    else
        eye = &scene.m[6];

    lookAtMatrix = lookat(eye->cd.v[P], eye->cd.v[U], eye->cd.v[V], eye->cd.v[N]);
    viewMatrix = inverse_mat(lookAtMatrix);

    if (!PROJECTIONVIEW) {
        worldMatrix = mxm(viewMatrix, perspMatrix);
    } else {
        worldMatrix = mxm(viewMatrix, orthoMatrix);
    }

    if (!DEBUG)
        glPolygonMode(GL_FRONT, GL_FILL);
    else if (DEBUG == 1)
        glPolygonMode(GL_FRONT, GL_LINE);
    else
        glPolygonMode(GL_FRONT, GL_POINT);

    GLfloat vpMatrix[16], mMatrix[16];
    memcpy(&vpMatrix, &worldMatrix, 64);

    glUniformMatrix4fv(0, 1, GL_FALSE, vpMatrix);

    for (int i = 0; i < scene.m_indexes; i++) {

        if ( frustumCulling(scene.m[i].bbox.v, scene.m[i].bbox.v_indexes, worldMatrix) ) {

            Mat4x4 sclMatrix, trMatrix;

            vec4f pos = { 0 };
            Mat4x4 mfQ = MatfromQuat(scene.m[i].Q, pos);
            sclMatrix = mxm(mfQ, scaleMatrix(scene.m[i].scale));
            trMatrix = translationMatrix(scene.m[i].cd.v[P][0], scene.m[i].cd.v[P][1], scene.m[i].cd.v[P][2]);
            scene.m[i].modelMatrix = mxm(sclMatrix, trMatrix);
            memcpy(&mMatrix, &scene.m[i].modelMatrix, 64);

            glUniformMatrix4fv(1, 1, GL_FALSE, mMatrix);
            glUniform1i(2, scene.m[i].id);

            glBufferData(GL_ARRAY_BUFFER, scene.m[i].vba_indexes * 32, scene.m[i].vba, GL_STATIC_DRAW);

            glDrawArrays(GL_TRIANGLES, 0, scene.m[i].vba_indexes);
        }
    }

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "clickSelect < %d >  ", err);
        perror("OpenGL ERROR: ");
    }

    if ( getClick ) {
        int data;
        glReadBuffer(GL_COLOR_ATTACHMENT1);
        glReadPixels(click[0], main_wa.height - click[1], 1, 1, GL_RED_INTEGER, GL_INT, &data);

        printf("mesh_id: %d\n", data);
        getClick = 0;
        mesh_id = data;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
const static void shadowCast() {

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

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "shadowCast < %d >  ", err);
        perror("OpenGL ERROR: ");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glXSwapBuffers(displ, mainwin);
}
const static void project(void) {

    glUseProgram(mainShaderProgram);

    glViewport(0, 0, WIDTH, HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
    glDrawBuffers(2, drawBuffers);
    glClear(GL_DEPTH_BUFFER_BIT);

    if (EYEPOINT)
        eye = &scene.m[7];
    else
        eye = &scene.m[6];

    lookAtMatrix = lookat(eye->cd.v[P], eye->cd.v[U], eye->cd.v[V], eye->cd.v[N]);
    viewMatrix = inverse_mat(lookAtMatrix);

    if (!PROJECTIONVIEW) {
        worldMatrix = mxm(viewMatrix, perspMatrix);
    } else {
        worldMatrix = mxm(viewMatrix, orthoMatrix);
    }

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

            glUniform1i(glGetUniformLocation(mainShaderProgram,  "v_index"), scene.m[i].tex_index);
            glUniform1i(3, scene.m[i].id);

            glBufferData(GL_ARRAY_BUFFER, scene.m[i].vba_indexes * 32, scene.m[i].vba, GL_STATIC_DRAW);

            glDrawArrays(GL_TRIANGLES, 0, scene.m[i].vba_indexes);
        }
    }

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "project < %d >  ", err);
        perror("OpenGL ERROR: ");
    }

    int data[2];
    if ( getClick ) {
        glReadBuffer(GL_COLOR_ATTACHMENT1);
        glReadPixels(click[0], main_wa.height - click[1], 1, 1, GL_RG_INTEGER, GL_INT, &data);

        printf("mesh_info[0]: %d    mesh_info[1]: %d\n", data[0], data[1]);
        getClick = 0;
        mesh_id = data[0];
        primitive_id = data[1];
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glXSwapBuffers(displ, mainwin);
    // displayPoint(scene.m[1].cd.v[P], worldMatrix, 0xff00a7); //0028ff
    face f = facexm(scene.m[mesh_id].f[primitive_id], scene.m[mesh_id].modelMatrix);
    displayFace(&f, worldMatrix); //0028ff
    // displayMeshKinetics(&scene.m[1], worldMatrix); //0028ff
    // displayBboxFaces(&scene.m[mesh_id], worldMatrix); //0028ff
}
const static void initMainWindow(void) {
    int screen = XDefaultScreen(displ);
    if ( (vinfo = glXChooseVisual(displ, screen, att)) == 0 ) {
        fprintf(stderr, "No matching visual. initMainWindow() -- glxChooseVisual().\n");
        exit(0);
    }

    /* The root window which is controled by the window manager. */
    root = DefaultRootWindow(displ);
    if ( (cmap = XCreateColormap(displ, root, vinfo->visual, AllocNone)) == 0 ) {
        fprintf(stderr, "Unable to create colormap. initMainWindow() -- XCreateColormap().\n");
        exit(0);
    }

    sa.event_mask = EXPOSEMASKS | KEYBOARDMASKS | POINTERMASKS;
    sa.background_pixel = 0x000000;
    sa.colormap = cmap;
    mainwin = XCreateWindow(displ, XRootWindow(displ, XDefaultScreen(displ)), 0, 0, WIDTH, HEIGHT, 0, vinfo->depth, InputOutput, vinfo->visual, CWBackPixel | CWEventMask | CWColormap, &sa);
    XMapWindow(displ, mainwin);
    XGetWindowAttributes(displ, mainwin, &main_wa);
}
////////////////////////////////////
//        SETUP GL CONTEXT        //
////////////////////////////////////
const static void setupGL(void) {
    /////////////////////////////////////////////////
    //	CREATE GL CONTEXT AND MAKE IT CURRENT	//
    /////////////////////////////////////////////////
    if ( (glc = glXCreateContext(displ, vinfo, NULL, GL_TRUE)) == 0 ) {
        fprintf(stderr, "Unable to create gl context. setupGL() -- glxCreateContext().\n");
        exit(0);
    }

    glXMakeCurrent(displ, mainwin, glc);
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // glEnable(GL_MULTISAMPLE);
    glClearColor(0.00, 0.00, 0.00, 1.00);

    /* Initialize Glew and check for Errors. */
    const GLenum err = glewInit();
    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Create a 2D Texture to use it as the depth buffer for the Shadow Map. */
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    glGenTextures(1, &shadowDepthMap);
    printf("shadowDepthMap: %d\n", shadowDepthMap);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, shadowDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Create a user specific framebuffer to use it for rendering instead of the default framebuffer.*/
    glGenTextures(1, &mainDepthMap);
    printf("mainDepthMap: %d\n", mainDepthMap);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, mainDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    /* Create a INFO texture.*/
    glGenTextures(1, &mainInfoMap);
    printf("mainInfoMap: %d\n", mainInfoMap);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, mainInfoMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32I, WIDTH, HEIGHT, 0, GL_RG_INTEGER, GL_INT, NULL);

    /* Attach the generated 2D Texture to our Shadow Map framebuffer's depth buffer */
    glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mainColorMap, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mainDepthMap, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mainInfoMap, 0);
    if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
        perror("Incomplete mainFBO ");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
const int assignUniformLocations(void) {
    if ( (transformLocA = glGetUniformLocation(mainShaderProgram, "vpMatrix")) != 0)
        fprintf(stderr, "Could not locate uniform variable with name: vpMatrix. Error: %d\n", transformLocA);

    if ( (transformLocB = glGetUniformLocation(mainShaderProgram, "lightSpaceMatrix")) != 1)
        fprintf(stderr, "Could not locate uniform variable with name: lightSpaceMatrix. Error: %d\n", transformLocB);

    if ( (transformLocC = glGetUniformLocation(mainShaderProgram, "modelMatrix")) != 2)
        fprintf(stderr, "Could not locate uniform variable with name: modelMatrix. Error: %d\n", transformLocC);

    if ( (transformLocD = glGetUniformLocation(mainShaderProgram, "lightPos")) != 3)
        fprintf(stderr, "Could not locate uniform variable with name: lightPos. Error: %d\n", transformLocD);

    if ( (transformLocE = glGetUniformLocation(mainShaderProgram, "cameraPos")) != 4)
        fprintf(stderr, "Could not locate uniform variable with name: cameraPos. Error: %d\n", transformLocE);

    if ( (transformLocF = glGetUniformLocation(mainShaderProgram, "shadowDepthMap")) != 5)
        fprintf(stderr, "Could not locate uniform variable with name: shadowDepthMap. Error: %d\n", transformLocF);

    if ( (transformLocG = glGetUniformLocation(shadowShaderProgram, "lightSpaceMatrix")) != 0)
        fprintf(stderr, "Could not locate uniform variable with name: vpMatrix. Error: %d\n", transformLocG);

    if ( (transformLocH = glGetUniformLocation(shadowShaderProgram, "modelMatrix")) != 1)
        fprintf(stderr, "Could not locate uniform variable with name: modelMatrix. Error: %d\n", transformLocH);

    // if ( (transformLocI = glGetUniformLocation(debugShaderProgram, "shadowDepthMap")) != 0)
    //     fprintf(stderr, "Could not locate uniform variable with name: shadowDepthMap. Error: %d\n", transformLocI);
}
const static void initThreads(void) {
    /* Init thread_ids dynamically */
    thread_ids = malloc(THREADS * 4);
    for (int i = 0; i < THREADS; i++)
        thread_ids[i] = i;
}
const static void initGlobalGC(void) {
    gcvalues.foreground = 0xffffff;
    gcvalues.background = 0x000000;
    gcvalues.graphics_exposures = False;
    gc = XCreateGC(displ, mainwin, GCBackground | GCForeground | GCGraphicsExposures, &gcvalues);
}
const static void initDependedVariables(void) {
    glViewport(0, 0, main_wa.width, main_wa.height);

    ASPECTRATIO = ((float)main_wa.width / (float)main_wa.height);

    half_screen[0] = main_wa.height >> 1;
    half_screen[1] = main_wa.width >> 1;

    /* Matrices initialization. */
    perspMatrix = perspectiveMatrix(FOV, ASPECTRATIO, ZNEAR, ZFAR);
    reperspMatrix = reperspectiveMatrix(FOV, ASPECTRATIO);
    orthoMatrix = orthographicMatrix(-10000.f, 10000.f, -10000.f, 10000.f, ZNEAR, -10000);
}
const static void initAtoms(void) {

    wmatom[Win_Close] = XInternAtom(displ, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(displ, mainwin, &wmatom[Win_Close], 1);

    wmatom[Win_Name] = XInternAtom(displ, "WM_NAME", False);
    wmatom[Atom_Type] =  XInternAtom(displ, "STRING", False);
    XChangeProperty(displ, mainwin, wmatom[Win_Name], wmatom[Atom_Type], 8, PropModeReplace, (unsigned char*)"Anvil", 5);
}
const static void announceReadyState(void) {
    printf("Announcing ready process state event\n");
    XEvent event = { 0 };
    event.xkey.type = KeyPress;
    event.xkey.keycode = None;  // 33 is p;
    event.xkey.display = displ;
    event.xkey.send_event = 1;

    /* Send the signal to our event dispatcher for further processing. */
    handler[event.type](&event);
}
const static void InitTimeCounter(void) {
    gettimeofday(&tv0, NULL);
}
const static void UpdateTimeCounter(void) {
    LastFrameTimeCounter = TimeCounter;
    gettimeofday(&tv, NULL);
    TimeCounter = (float)(tv.tv_sec - tv0.tv_sec) + 0.000001 * ((float)(tv.tv_usec - tv0.tv_usec));
    DeltaTime = TimeCounter - LastFrameTimeCounter;
}
const static void CalculateFPS(void) {
    Frame++;
    if ( (Frame % 30) == 0 )
        FPS = 1.f / DeltaTime;
    prevTime = TimeCounter;
}
const static void displayInfo(void) {
    char info_string[64];

    time_t t = tv.tv_sec;
    struct tm *info;
    info = localtime(&t);

    sprintf(info_string, "Resolution   : %d x %d", main_wa.width, main_wa.height);
    XDrawString(displ ,mainwin ,gc, 5, 12, info_string, strlen(info_string));

    sprintf(info_string, "Running Time : %4.1f", TimeCounter);
    XDrawString(displ ,mainwin ,gc, 5, 24, info_string, strlen(info_string));

    sprintf(info_string, "FPS          : %4.1f", FPS);
    XDrawString(displ ,mainwin ,gc, 5, 36, info_string, strlen(info_string));

    sprintf(info_string, "Frames Drawn : %d", Frame);
    XDrawString(displ ,mainwin ,gc, 5, 48, info_string, strlen(info_string));

    sprintf(info_string, "DeltaTime    : %f ", DeltaTime);
    XDrawString(displ ,mainwin ,gc, 5, 60, info_string, strlen(info_string));

    sprintf(info_string, "Date Time    : %s ", asctime(info));
    XDrawString(displ ,mainwin ,gc, 5, 72, info_string, strlen(info_string));

    // sprintf(info_string, "Camera x: %f,    y: %f,    z: %f\0", camera[0][0], camera[0][1], camera[0][2]);
    // XDrawString(displ ,mainwin ,gc, 5, 60, info_string, strlen(info_string));
}
/* Signal handler to clean memory before exit, after receiving a given signal. */
const static void sigsegv_handler(const int sig) {
    printf("Received Signal from OS with ID: %d\n", sig);
    XEvent event = { 0 };
    event.type = ClientMessage;
    event.xclient.data.l[0] = wmatom[Win_Close];
    /* Send the signal to our event dispatcher for further processing. */
    if(RUNNING)
        handler[event.type](&event);
}
/* Registers the given Signal. */
const static int registerSig(const int signal) {
    /* Signal handling for effectivelly releasing the resources. */
    struct sigaction sig = { 0 };
    sig.sa_handler = &sigsegv_handler;
    int sig_val = sigaction(signal, &sig, NULL);
    if (sig_val == -1) {
        fprintf(stderr, "Warning: board() -- sigaction()\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
static int predicate(Display *d, XEvent *e, XPointer arg) {
    return (e->type == KeyPress);
}
/* General initialization and event handling. */
const static int board(void) {

    initGlobalGC();
    initAtoms();
    // registerSig(SIGSEGV);

    initDependedVariables();

    createScene(&scene);     /*  Scene creation must happen after world objects initialization.    */
    initWorldObjects(&scene);

    announceReadyState();

    float time_dif;
    while(RUNNING) {

        UpdateTimeCounter();
        CalculateFPS();
        displayInfo();
        // clock_t start_time = start();
        applyPhysics(&scene);
        // printf("Apply Physics  : %f\n", end(start_time));
        // clock_t start_time = start();
        shadowCast();
        // clickSelect();
        // displayTexture(6);
        project();
        displayTexture(5);
        // printf("Project     : %f\n", end(start_time));;

        while ( XPending(displ) ) {
            XEvent event;

            XNextEvent(displ, &event);
            if ( event.type == KeyRelease ) {
                XEvent cache_1 = { 0 };

                usleep(500);
                if ( XEventsQueued(displ, QueuedAfterReading) ) {

                    XPeekEvent(displ, &cache_1);
                    if ( cache_1.type == KeyPress && (cache_1.xkey.serial == event.xkey.serial) && (cache_1.xkey.time == event.xkey.time) ) {
                        XNextEvent(displ, &event);
                        // is_repeated = 1;
                    }
                }
            }

            if (handler[event.type])
                handler[event.type](&event);
        }
        time_dif = DeltaTime > 0.016666 ? 0 : (0.016666 - DeltaTime) * 100000;
        usleep(time_dif);
    }
    return EXIT_SUCCESS;
}
const int main(int argc, char *argv[]) {
    if (argc > 1) {
        printf("argc: %d\n", argc);
        if (strcasecmp(argv[1], "Debug") == 0) {
            if (strcasecmp(argv[2], "1") == 0) {
                fprintf(stderr, "INFO : ENABLED DEBUG Level 1\n");
                DEBUG = 1;
            } else if (strcasecmp(argv[2], "2") == 0) {
                fprintf(stderr, "INFO : ENABLED DEBUG Level 2\n");
                DEBUG = 2;
            }
        }
    }

    // if (!XInitThreads()) {
    //     fprintf(stderr, "Warning: board() -- XInitThreads()\n");
    //     return EXIT_FAILURE;
    // }
    // XLockDisplay(displ);

    displ = XOpenDisplay(NULL);
    if (displ == NULL) {
        fprintf(stderr, "Warning: board() -- XOpenDisplay()\n");
        return EXIT_FAILURE;
    }

    initMainWindow();

    setupGL();
    createBuffers();
    createTextures();
    mainShaderProgram = initMainShader();
    shadowShaderProgram = initShadowShader();
    debugShaderProgram = initDebugShader();
    clickSelectShaderProgram = initClickSelectShader();
    glUseProgram(mainShaderProgram);
    // assignUniformLocations();

    InitTimeCounter();

    createMaterialDatabase();

    // initThreads();

    if (board()) {
        fprintf(stderr, "ERROR: main() -- board()\n");
        return EXIT_FAILURE;
    }

    // free(thread_ids);
    XCloseDisplay(displ);
    
    return EXIT_SUCCESS;
}



