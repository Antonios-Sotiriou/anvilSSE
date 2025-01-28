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

/* Shader Programms Glodal ids. */
int mainShaderProgram, shadowShaderProgram, displayShaderProgram, clickSelectShaderProgram;

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
#include "headers/shaders/displayShader.h"
#include "headers/shaders/clickSelectShader.h"
#include "headers/components/anvil_structs.h"
#include "headers/components/opengl_config.h"
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


// typedef enum { mainInfoMap } TextureUnit;
#define TEXTURE_0    GL_TEXTURE0
typedef struct {
    int id, index, name;
} TextureUnit;

/* Project Global Variables. */
int PROJECTIONVIEW        = 0;
static int PROJECTBUFFER  = 5;
int EYEPOINT              = 6;
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
int COLLISION             = 0;
int COLLIDINGFACE         = 0;
int COLLIDINGMESH         = 0;
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

vec4f gravity_epicenter = { 0.f, -1.f, 0.f };
const float sunMov = 100.0f;
float movScalar = 2.f;
const float moveForce = 10.f;
vec4f point = { 1500.000000, -0.000055, 1700.000000, 1.000000 };

/* Variables usefull for mesh click select. */
int mesh_id = 0, primitive_id = 0;
vec4f click = { 0 };

/* Global Matrices */
Mat4x4 perspMatrix, lookAtMatrix, viewMatrix, reperspMatrix, orthoMatrix, worldMatrix, lightMatrix, ortholightMat[NUM_OF_CASCADES], *point_mat; 

/* Anvil global Objects Meshes and Scene. */
Scene scene = { 0 };

/* X11 and mainwindow Global variables. */
static int INIT = 0;
static int RUNNING = 1;
vec4i half_screen = { 0, 0, 1, 1 }; // This variable is initialized in configurenotify function.Its Helping us decrease the number of divisions.
int DEBUG = 0, UPDATE = 1;

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
const static void initGlobalGC(void);
const static void initDependedVariables(void);
const static void initAtoms(void);
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

        for (int i = 0; i < scene.t.quadsArea; i++) {
            if (scene.t.quads[i].members)
                free(scene.t.quads[i].members);
        }
        free(scene.t.quads);

        releaseScene(&scene);

        XFreeGC(displ, gc);

        // 4. Unbinding the Vertex and Buffer arrays.
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        glDeleteBuffers(1, &VBO);

        glDeleteTextures(4, testTexture);

        glDeleteFramebuffers(1, &shadowMapFBO);
        glDeleteTextures(1, &shadowDepthMap);

        glDeleteFramebuffers(1, &mainFBO);
        glDeleteTextures(1, &mainColorMap);
        glDeleteTextures(1, &mainDepthMap);
        glDeleteTextures(1, &mainInfoMap);

        glDeleteProgram(mainShaderProgram);
        glDeleteProgram(shadowShaderProgram);
        glDeleteProgram(displayShaderProgram);
        glDeleteProgram(clickSelectShaderProgram);

        glXMakeCurrent(displ, None, NULL);
        XFree(vinfo);
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

        if ( INIT ) {

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

    int data[2];
    glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glReadPixels(click[0], main_wa.height - click[1], 1, 1, GL_RG_INTEGER, GL_INT, &data);

    printf("mesh_info[0]: %d    mesh_info[1]: %d\n", data[0], data[1]);
    mesh_id = data[0];
    primitive_id = data[1];
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    vec4f normal = { -1.f, 0.f, 0.f, 0.f};
    // point = { 1500.000000, -0.000055, 1700.000000, 1.000000 };
    vec4f edge1 = (vec4f){ 1500.000000, 0.000000, 1500.000000, 1.000000 } - (vec4f){ 1500.000000, 1000.000000, 2500.000000, 1.000000 };
    vec4f edge2 = (vec4f){ 1500.000000, 0.000000, 1500.000000, 1.000000 } - point;
    printf("E: %f\n", dot_product(normal, cross_product(edge1, edge2)));  
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
        case 65450 : point[1] += 0.0001f;             /* * */
            logVec4f(point);
            break;
        case 65455 : point[1] -= 0.0001f;             /* / */
            logVec4f(point);
            break;
        case 65430 : //sunlight.pos[0] -= sunMov;                   /* Adjust Light Source */
            // vec4f mva = norm_vec(camera[N] - camera[U]);
            vec4f mva = { -1.f, 0.f, 0.f };
            scene.m[1].mvdir = mva;
            scene.m[1].velocity = mva * movScalar;
            scene.m[1].roll = 1;
            break;
        case 65432 : //sunlight.pos[0] += sunMov;                   /* Adjust Light Source */
            // vec4f mvb = -norm_vec(camera[N] - camera[U]);
            // vec4f mvb = norm_vec(scene.m[Camera_1].cd.v[U] + scene.m[Camera_1].cd.v[V]);
            vec4f mvb = { 1.f, 0.f, 0.f };
            scene.m[1].velocity = mvb * movScalar;
            scene.m[1].roll = 1;
            break;
        case 65434 : //sunlight.pos[1] += sunMov;                   /* Adjust Light Source */
            scene.m[1].grounded = 0;
            vec4f mvc = { 0.f, 1.f, 0.f };
            scene.m[1].velocity = mvc * (movScalar * scene.m[1].mass);
            scene.m[1].falling_time = 0.f;
            break;
        case 65435 : //sunlight.pos[1] -= sunMov;                   /* Adjust Light Source */
            vec4f mvd = { 0.f, -1.f, 0.f };
            scene.m[1].velocity = mvd  * (movScalar + 0.981f * scene.m[1].mass);
            break;
        case 65431 : //sunlight.pos[2] += sunMov;                   /* Adjust Light Source */
            // vec4f mve = norm_vec(scene.m[Camera_1].cd.v[U] + scene.m[Camera_1].cd.v[N]);
            vec4f mve = { 0.5f, 0.f, 0.5f };
            scene.m[1].velocity = mve * movScalar;
            scene.m[1].roll = 1;
            break;
        case 65433 : //sunlight.pos[2] -= sunMov;                   /* Adjust Light Source */
            // vec4f mvf = -norm_vec(scene.m[Camera_1].cd.v[U] + scene.m[Camera_1].cd.v[N]);
            vec4f mvf = { -0.5f, 0.f, -0.5f };
            scene.m[1].velocity = mvf * movScalar;
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
            if ( mesh_id > 0 ) {
                rotateMesh(&scene.m[mesh_id], 10, 0.f, 1.f, 0.0f);
                rotateMesh(&scene.m[mesh_id], 10, 1.f, 0.f, 0.0f);
            }
            break;
        case 43 : movScalar += 500;                                    /* + */
            printf("movScalar: %f\n", movScalar);
            break;
        case 45 : movScalar -= 500;                                   /* - */
            printf("movScalar: %f\n", movScalar);
            break;
        case 112 :
            if (PROJECTBUFFER == 7)
                PROJECTBUFFER = 0;
            else
                PROJECTBUFFER++;
            fprintf(stderr, "TextureUnit: %d\n", PROJECTBUFFER);
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
            if (EYEPOINT == 7)
                EYEPOINT = 6;
            else
                EYEPOINT = 7;
            break;
        case 118 :
            if (!PROJECTIONVIEW)               /* v */
                PROJECTIONVIEW++;
            else
                PROJECTIONVIEW = 0;
            break;
    }

    UPDATE = 1;
}
const static void keyrelease(XEvent *event) {

    KeySym keysym = XLookupKeysym(&event->xkey, 0);

    // printf("Key Released: %ld\n", keysym);
    scene.m[EYEPOINT].velocity -= scene.m[EYEPOINT].velocity;
    scene.m[EYEPOINT].rot_angle = 0;

    scene.m[1].velocity -= scene.m[1].velocity;

    if ( keysym == 99 )
        scene.m[mesh_id].rot_angle = 0;
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

    // announceReadyState();

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
        displayTexture(PROJECTBUFFER);
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
    displayShaderProgram = initDisplayShader();
    clickSelectShaderProgram = initClickSelectShader();

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



