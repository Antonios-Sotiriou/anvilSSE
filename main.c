/* General Macro definitions */
#define _GNU_SOURCE /* Importand to counter sigaction struct < incomplete type error >. */

/* general headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <sys/time.h>

/* signal */
#include <signal.h>
// #include <immintrin.h>

/* testing */
#include "headers/exec_time.h"
#include "headers/logging.h"
#include "headers/test_shapes.h"

/* ############################################## MULTITHREADING ################################################################### */
#include <pthread.h>
#define THREADS 8
pthread_t threads[THREADS];
int *thread_ids;
/* ############################################## MULTITHREADING ################################################################### */

int EDGEFUNC = 0;
int SCANLINE = 1;

/* Project specific headers */
#include "headers/database.h"
#include "headers/anvil_structs.h"
#include "headers/matrices.h"
#include "headers/kinetics.h"
#include "headers/clipping.h"
#include "headers/shadow_pipeline.h"
#include "headers/grafik_pipeline.h"
#include "headers/camera.h"
#include "headers/world_objects.h"
#include "headers/general_functions.h"
#include "headers/draw_functions.h"

enum { Win_Close, Win_Name, Atom_Type, Atom_Last};
enum { Pos, U, V, N, C, newPos };

#define WIDTH                     1000
#define HEIGHT                    1000
#define MAP_WIDTH                 200
#define MAP_HEIGHT                200
#define POINTERMASKS              ( ButtonPressMask )
#define KEYBOARDMASKS             ( KeyPressMask )
#define EXPOSEMASKS               ( StructureNotifyMask )
#define NUM_OF_CASCADES           3

/* X Global Structures. */
Display *displ;
Window mainwin, mapwin;
XImage *main_image, *map_image;
Pixmap main_pixmap, map_pixmap;
GC gc;
XGCValues gcvalues;
XWindowAttributes main_wa, map_wa, *point_attrib;
XSetWindowAttributes sa;
Atom wmatom[Atom_Last];

/* BUFFERS. */
u_int8_t *frame_buffer, *map_buffer, *point_buffer, *reset_buffer;
float *main_depth_buffer, *map_depth_buffer, *point_depth_buffer, *shadow_buffer[NUM_OF_CASCADES];
Fragment *frags_buffer, *reset_frags;

/* Project Global Variables. */
int PROJECTIONVIEW = 0;
static int PROJECTBUFFER  = 1;
static int AdjustShadow   = 1;
static int AdjustScene    = 1;
static int EYEPOINT       = 0;
static float FOV          = 45.0f;
static float ZNEAR        = 0.1f;
static float ZFAR         = 1.0f;
static float ASPECTRATIO  = 1;
static int FBSIZE         = 0;
float NPlane              = 1.0f;
float FPlane              = 20000.0f;
float SCALE               = 0.003f;
float AmbientStrength     = 0.85f;
float SpecularStrength    = 0.75f;
float shadow_bias         = 0.f;//0.003105;//0.002138;//0.000487f;

/* Camera and Global light Source. */
vec4f *eye;
vec4f camera[N + 1] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f }
};
Light sunlight = {
    // .pos = { -800.001343f, 205.598007f, 802.004822f, 1.000000f },
    // .u = { -0.694661f, 0.000000f, -0.719352f, 0.000000f },
    // .v = { 0.000000f, -1.000000f, 0.000000f, 0.000000f },
    // .n = { 0.719352f, 0.000000f, -0.694661f, 0.000000f },
    .pos = { 0.f, 100.0f, 0.f, 1.f },
    .u = { 1.f, 0.f, 0.f, 0.f },
    .v = { 0.f, 0.f, -1.f, 0.f },
    .n = { 0.f, -1.f, 0.f, 0.f },
};

/* Global Matrices */
Mat4x4 perspMat, lookAt, viewMat, reperspMat, orthoMat, worldMat, ortholightMat[3], persplightMat, lm, lview, *point_mat;

/* Anvil global Objects Meshes and Scene. */
Scene scene = { 0 };

/* X11 and mainwindow Global variables. */
static int INIT = 0;
static int RUNNING = 1;
int HALFW = 0; // Half width of the screen; This variable is initialized in configurenotify function.Its Helping us decrease the number of divisions.
int HALFH = 0; // Half height of the screen; This variable is initialized in configurenotify function.Its Helping us decrease the number of divisions.
int MAIN_EMVADON, MAP_EMVADON;
int DEBUG = 0;

/* Display usefull measurements. */
float			        TimeCounter, LastFrameTimeCounter, DeltaTime, prevTime = 0.0, FPS;
struct timeval		    tv, tv0;
int			            Frame = 1, FramesPerFPS;

/* Event handling functions. */
const static void clientmessage(XEvent *event);
const static void reparentnotify(XEvent *event);
const static void mapnotify(XEvent *event);
const static void resizerequest(XEvent *event);
const static void configurenotify(XEvent *event);
const static void buttonpress(XEvent *event);
const static void keypress(XEvent *event);

/* Represantation functions */
const static void project(void);
const static void drawFrame(void);

/* Xlib relative functions and event dispatcher. */
const static void initMainWindow(void);
const static void initGlobalGC(void);
const static void initDependedVariables(void);
const static void initAtoms(void);
const static void initBuffers(void);
const static void initLightModel(Light *l);
const static void pixmapcreate(void);
const static void pixmapdisplay(Drawable pixmap, Drawable window, unsigned int wdth, unsigned int heigth);
const static void announceReadyState(void);
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
};
/* ################################################### CASCADE SHADOW MAPPING ################################################ */
/* Returns a vec4f array with the values of the 8 vectors that form the View Frustum in View Space.np and fp are the near and far planes( can be Choosen accordingly ). */
vec4f *worldSpaceFrustum(const float np, const float fp) {
    vec4f *va = malloc(128);
    const float fovRadius = (1.f / tanf(45.f * 0.5f / 180.0f * 3.14159f));

    const vec4f nearcenter = (eye[0] + eye[3]) * np;
    const vec4f farcenter = (eye[0] + eye[3]) * fp;

    const float nearHeight = tan(fovRadius) * np;
    const float farHeight = tan(fovRadius) * fp;
    const float nearWidth = nearHeight * ASPECTRATIO;
    const float farWidth = farHeight * ASPECTRATIO;

    const vec4f yxnh = eye[2] * nearHeight;
    const vec4f yxnw = eye[1] * nearWidth;

    const vec4f yxfh = eye[2] * farHeight;
    const vec4f yxfw = eye[1] * farWidth;

    va[2] = nearcenter + yxnh + yxnw;
    va[3] = nearcenter - yxnh + yxnw;
    va[6] = nearcenter + yxnh - yxnw;
    va[7] = nearcenter - yxnh - yxnw;

    va[0] = farcenter + yxfh + yxfw;
    va[1] = farcenter - yxfh + yxfw;
    va[4] = farcenter + yxfh - yxfw;
    va[5] = farcenter - yxfh - yxfw;

    // va = setvecsarrayxm(va, 8, viewMat);

    return va;
}
/* Finds the minX, maxX, minY, maxY, minZ maxZ values of given vectors array. AKA (bounding box). */
const DimensionsLimits getDimensionsLimits(vec4f va[]) {
    DimensionsLimits dl = { 0 };
    for (int i = 0; i < 8; i++) {
        /* Get min and max x values. */
        if ( va[i][0] <= dl.minX) {
            dl.minX = va[i][0];
        } else if ( va[i][0] > dl.maxX) {
            dl.maxX = va[i][0];             
        }
        /* Get min and max y values. */
        if ( va[i][1] <= dl.minY) {
            dl.minY = va[i][1];
        } else if ( va[i][1] > dl.maxY) {
            dl.maxY = va[i][1];             
        }
        /* Get min and max z values. */
        if ( va[i][2] <= dl.minZ) {
            dl.minZ = va[i][2];
        } else if ( va[i][2] > dl.maxZ) {
            dl.maxZ = va[i][2];             
        }
    }
    return dl;
}
const Mat4x4 createOrthoMatrixFromLimits(const DimensionsLimits dl) {
    return orthographicMatrix(dl.minX, dl.maxX, dl.minY, dl.maxY, dl.minZ, dl.maxZ);
}
const static void createCascadeShadowMatrices(const unsigned int num_of_cascades) {
    DimensionsLimits dl;
    vec4f *fr[3] = {
        worldSpaceFrustum(NPlane, 100.f),
        worldSpaceFrustum(100.f, 300.f),
        worldSpaceFrustum(300.f, 600.f)
    };
    Mat4x4 lm[3] = {
        lookat(camera[Pos] + (sunlight.pos + (camera[3] * 100.f)), sunlight.u, sunlight.v, sunlight.n),//100.f
        lookat(camera[Pos] + (sunlight.pos + (camera[3] * 300.f)), sunlight.u, sunlight.v, sunlight.n),//460.f
        lookat(camera[Pos] + (sunlight.pos + (camera[3] * 600.f)), sunlight.u, sunlight.v, sunlight.n)//1260.f
    };

    for (int i = 0; i < num_of_cascades; i++) {
        lm[i].m[3][1] = sunlight.pos[1];
        lview = inverse_mat(lm[i]);
        /* Transform view frustum to Space. */
        fr[i] = setvecsarrayxm(fr[i], 8, viewMat);

        dl = getDimensionsLimits(fr[i]);
        free(fr[i]);

        ortholightMat[i] = mxm(lview, createOrthoMatrixFromLimits(dl));
    }
}
/* ################################################### CASCADE SHADOW MAPPING ################################################ */
const static void clientmessage(XEvent *event) {
    printf("Received client message event\n");
    if (event->xclient.data.l[0] == wmatom[Win_Close]) {

        releaseScene(&scene);

        free(frame_buffer);
        free(map_buffer);
        free(main_depth_buffer);
        free(map_depth_buffer);
        free(reset_buffer);

        free(frags_buffer);
        free(reset_frags);
        free(thread_ids);

        free(shadow_buffer[0]);
        free(shadow_buffer[1]);
        free(shadow_buffer[2]);

        free(main_image);
        free(map_image);
        XFreeGC(displ, gc);
        XFreePixmap(displ, main_pixmap);
        XFreePixmap(displ, map_pixmap);
        XDestroyWindow(displ, mapwin);
        XDestroyWindow(displ, mainwin);

        RUNNING = 0;
    }
}
const static void reparentnotify(XEvent *event) {

    printf("reparentnotify event received\n");
}
const static void mapnotify(XEvent *event) {

    printf("mapnotify event received\n");
}
const static void resizerequest(XEvent *event) {

    printf("resizerequest event received\n");
}
const static void configurenotify(XEvent *event) {

    if (!event->xconfigure.send_event) {
        printf("configurenotify event received\n");
        XGetWindowAttributes(displ, mainwin, &main_wa);
        XGetWindowAttributes(displ, mapwin, &map_wa);

        if (INIT) {

            free(frame_buffer);
            free(map_buffer);
            free(main_depth_buffer);
            free(map_depth_buffer);
            free(reset_buffer);

            free(frags_buffer);
            free(reset_frags);
            free(thread_ids);

            free(shadow_buffer[0]);
            free(shadow_buffer[1]);
            free(shadow_buffer[2]);

            free(main_image);
            free(map_image);

            initDependedVariables();

            initBuffers();
            pixmapcreate();
        } else {
            INIT = 1;
        }
    }
}
const static void buttonpress(XEvent *event) {

    printf("buttonpress event received\n");
    printf("X: %f\n", ((event->xbutton.x - (WIDTH / 2.00)) / (WIDTH / 2.00)));
    printf("Y: %f\n", ((event->xbutton.y - (HEIGHT / 2.00)) / (HEIGHT / 2.00)));
}
const static void keypress(XEvent *event) {
    
    KeySym keysym = XLookupKeysym(&event->xkey, 0);

    if (EYEPOINT)
        eye = (vec4f*)&sunlight;
    else
        eye = (vec4f*)&camera;

    // printf("Key Pressed: %ld\n", keysym);
    printf("\x1b[H\x1b[J");
    // system("clear\n");
    switch (keysym) {
        case 97 : look_left(eye, 0.2);             /* a */
            rotate_light_cam(&scene.m[2], camera[0], 2.0f, 0.0f, 1.0f, 0.0f);
            // rotate_light(&sunlight, camera[0], 2.0f, 0.0f, 1.0f, 0.0f);
            break;
        case 100 : look_right(eye, 0.2);           /* d */
            rotate_light_cam(&scene.m[2], camera[0], -2.0f, 0.0f, 1.0f, 0.0f);
            // rotate_light(&sunlight, camera[0], -2.0f, 0.0f, 1.0f, 0.0f);
            break;
        case 113 : look_up(eye, 2.2);              /* q */
            break;
        case 101 : look_down(eye, 2.2);            /* e */
            break;
        case 119 : move_forward(eye, 12.2);         /* w */
            break;
        case 115 : move_backward(eye, 12.2);        /* s */
            break;
        case 65361 : move_left(eye, 12.2);          /* left arrow */
            break;
        case 65363 : move_right(eye, 12.2);         /* right arrow */
            break;
        case 65362 : move_up(eye, 10.2);            /* up arror */
            break;
        case 65364 : move_down(eye, 10.2);          /* down arrow */
            break;
        case 65451 :shadow_bias += 0.0001f;             /* + */
            printf("shadow_bias: %f\n",shadow_bias);
            break;
        case 65453 :shadow_bias -= 0.0001f;             /* - */
            printf("shadow_bias: %f\n", shadow_bias);
            break;
        case 65450 : NPlane += 1.f;             /* * */
            printf("NPlane: %f\n", NPlane);
            break;
        case 65455 : NPlane -= 1.f;             /* / */
            printf("NPlane: %f\n", NPlane);
            break;
        case 65430 : sunlight.pos[0] -= 10.0f;                   /* Adjust Light Source */
            Mat4x4 ar = translationMatrix(-10.0f, 0.0f, 0.0f);
            scene.m[2].v = setvecsarrayxm(scene.m[2].v, scene.m[2].v_indexes, ar);
            scene.m[2].pivot[0] -= 10.0f;
            break;
        case 65432 : sunlight.pos[0] += 10.0f;                   /* Adjust Light Source */
            Mat4x4 br = translationMatrix(10.0f, 0.0f, 0.0f);
            scene.m[2].v = setvecsarrayxm(scene.m[2].v, scene.m[2].v_indexes, br);
            scene.m[2].pivot[0] += 10.0f;
            break;
        case 65434 : sunlight.pos[1] += 10.0f;                   /* Adjust Light Source */
            Mat4x4 er = translationMatrix(0.0f, 10.0f, 0.0f);
            scene.m[2].v = setvecsarrayxm(scene.m[2].v, scene.m[2].v_indexes, er);
            scene.m[2].pivot[1] += 10.0f;
            break;
        case 65435 : sunlight.pos[1] -= 10.0f;                   /* Adjust Light Source */
            Mat4x4 fr = translationMatrix(0.0f, -10.0f, 0.0f);
            scene.m[2].v = setvecsarrayxm(scene.m[2].v, scene.m[2].v_indexes, fr);
            scene.m[2].pivot[1] -= 10.0f;
            break;
        case 65431 : sunlight.pos[2] += 10.0f;                   /* Adjust Light Source */
            Mat4x4 cr = translationMatrix(0.0f, 0.0f, 10.0f);
            scene.m[2].v = setvecsarrayxm(scene.m[2].v, scene.m[2].v_indexes, cr);
            scene.m[2].pivot[2] += 10.0f;
            break;
        case 65433 : sunlight.pos[2] -= 10.0f;                   /* Adjust Light Source */
            Mat4x4 dr = translationMatrix(0.0f, 0.0f, -10.0f);
            scene.m[2].v = setvecsarrayxm(scene.m[2].v, scene.m[2].v_indexes, dr);
            scene.m[2].pivot[2] -= 10.0f;
            break;
        case 120 : rotate_x(&scene.m[0], 1);                     /* x */
            break;
        case 121 : rotate_y(&scene.m[0], 1);                     /* y */
            break;
        case 122 : rotate_z(&scene.m[0], 1);                     /* z */
            break;
        case 114 : 
            vec4f center = { 0.f, 0.f, 0.f, 0.f };
            rotate_light(&sunlight, center, 1, 0.0f, 1.0f, 0.0f);        /* r */
            break;
        case 99 : rotate_origin(&scene.m[1], 1, 1.0f, 0.0f, 0.0f);  /* c */
            break;
        case 43 : SCALE += 0.0101;                                    /* + */
            orthoMat = orthographicMatrix(SCALE, SCALE, 0.0f, 0.0f, 0.01f, 0.1f);
            break;
        case 45 : SCALE -= 0.0101;                                   /* - */
            orthoMat = orthographicMatrix(SCALE, SCALE, 0.0f, 0.0f, 0.01f, 0.1f);
            break;
        case 112 :
            if (PROJECTBUFFER == 5)
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
            announceReadyState();
            return;
        case 118 :
            if (!PROJECTIONVIEW)               /* v */
                PROJECTIONVIEW++;
            else
                PROJECTIONVIEW = 0;
            break;
    }

    lookAt = lookat(eye[Pos], eye[U], eye[V], eye[N]);
    viewMat = inverse_mat(lookAt);
    sunlight.newPos = vecxm(sunlight.pos, viewMat);

    createCascadeShadowMatrices(NUM_OF_CASCADES);

    if (!PROJECTIONVIEW)
        worldMat = mxm(viewMat, perspMat);
    else
        worldMat = mxm(viewMat, orthoMat);

    // scene.m[0].v = worldSpaceFrustum(NPlane, 100.f);
    // scene.m[1].v = worldSpaceFrustum(100.f, 300.f);
    // scene.m[2].v = worldSpaceFrustum(300.f, 600.f);

    AdjustShadow++;
    AdjustScene++;
}
static void *oscillator(void *args) {

    int thread_id = *(int*)args;

    int ypol = ( (MAIN_EMVADON) % THREADS);
    int stuck = ( (MAIN_EMVADON) / THREADS);

    int tile_size = stuck * (thread_id + 1);
    int step = stuck * thread_id;

    if ( ypol && (thread_id == THREADS) )
        tile_size += ypol;

    for (int i = step; i < tile_size; i++) {
        if ( frags_buffer[i].state )
            phong(&frags_buffer[i]);
    }

    return (void*) args;
}
static void *cascade(void *args) {

    int shadow_id = *(int*)args;

    memcpy(shadow_buffer[shadow_id], reset_buffer, FBSIZE);
    shadowPipeline(&scene, shadow_id);

    return (void*) args;
}
const static void project() {
    if (AdjustShadow) {
        int shadow_ids[NUM_OF_CASCADES] = { 0, 1, 2 };
        for (int i = 0; i < NUM_OF_CASCADES; i++) {
            if (pthread_create(&threads[i], NULL, &cascade, &shadow_ids[i]))
                fprintf(stderr, "ERROR: project() -- cascade -- pthread_create()\n");
        }
        for (int i = 0; i < NUM_OF_CASCADES; i++) {
            if (pthread_join(threads[i], NULL))
                fprintf(stderr, "ERROR: project() -- cascade -- pthread_join()\n");
        }
        AdjustShadow = 0;
    }

    // if (AdjustScene) {
    //     memcpy(frame_buffer, reset_buffer, FBSIZE);
    //     memcpy(main_depth_buffer, reset_buffer, FBSIZE);
    //     memcpy(frags_buffer, reset_frags, MAIN_EMVADON * sizeof(Fragment));

        grafikPipeline(&scene);

        for (int i = 0; i < THREADS; i++) {
            if (pthread_create(&threads[i], NULL, &oscillator, &thread_ids[i]))
                fprintf(stderr, "ERROR: project() -- oscillator -- pthread_create()\n");
        }
        for (int i = 0; i < THREADS; i++) {
            if (pthread_join(threads[i], NULL))
                fprintf(stderr, "ERROR: project() -- oscillator -- pthread_join()\n");
        }
    //     AdjustScene = 0;
    // }

    drawFrame();
}
/* Writes the final Pixel values on screen. */
const static void drawFrame(void) {
    map_image->data = (char*)map_buffer;
    XPutImage(displ, map_pixmap, gc, map_image, 0, 0, 0, 0, map_wa.width, map_wa.height);
    pixmapdisplay(map_pixmap, mapwin, map_wa.width, map_wa.height);

    if (PROJECTBUFFER <= 1)
        main_image->data = (char*)frame_buffer;
    else if (PROJECTBUFFER == 2)
        main_image->data = (char*)main_depth_buffer;
    else if (PROJECTBUFFER == 3)
        main_image->data = (char*)shadow_buffer[0];
    else if (PROJECTBUFFER == 4)
        main_image->data = (char*)shadow_buffer[1];
    else if (PROJECTBUFFER == 5)
        main_image->data = (char*)shadow_buffer[2];

    XPutImage(displ, main_pixmap, gc, main_image, 0, 0, 0, 0, main_wa.width, main_wa.height);
    pixmapdisplay(main_pixmap, mainwin, main_wa.width, main_wa.height);

    memcpy(frame_buffer, reset_buffer, FBSIZE);
    memcpy(map_buffer, reset_buffer, MAP_EMVADON * 4);

    memcpy(main_depth_buffer, reset_buffer, FBSIZE);
    memcpy(map_depth_buffer, reset_buffer, MAP_EMVADON * 4);

    memcpy(frags_buffer, reset_frags, MAIN_EMVADON * sizeof(Fragment));
}
const static void initMainWindow(void) {
    sa.event_mask = EXPOSEMASKS | KEYBOARDMASKS | POINTERMASKS;
    sa.background_pixel = 0x000000;
    mainwin = XCreateWindow(displ, XRootWindow(displ, XDefaultScreen(displ)), 0, 0, WIDTH, HEIGHT, 0, CopyFromParent, InputOutput, CopyFromParent, CWBackPixel | CWEventMask, &sa);
    XMapWindow(displ, mainwin);
    XGetWindowAttributes(displ, mainwin, &main_wa);
}
const static void initMapWindow(void) {
    sa.event_mask = NoEventMask;
    sa.background_pixel = 0x000000;
    mapwin = XCreateWindow(displ, mainwin, WIDTH - MAP_WIDTH, 0, MAP_WIDTH, MAP_HEIGHT, 0, CopyFromParent, InputOutput, CopyFromParent, CWBackPixel | CWEventMask, &sa);
    XMapWindow(displ, mapwin);
    XGetWindowAttributes(displ, mapwin, &map_wa);
}
const static void initGlobalGC(void) {
    gcvalues.foreground = 0xffffff;
    gcvalues.background = 0x000000;
    gcvalues.graphics_exposures = False;
    gc = XCreateGC(displ, mainwin, GCBackground | GCForeground | GCGraphicsExposures, &gcvalues);
}
const static void initDependedVariables(void) {
    main_image = XCreateImage(displ, main_wa.visual, main_wa.depth, ZPixmap, 0, (char*)point_buffer, main_wa.width, main_wa.height, 32, (main_wa.width * 4));
    map_image = XCreateImage(displ, map_wa.visual, map_wa.depth, ZPixmap, 0, (char*)point_buffer, map_wa.width, map_wa.height, 32, (map_wa.width * 4));

    ASPECTRATIO = ((float)main_wa.width / (float)main_wa.height);
    HALFH = main_wa.height >> 1;
    HALFW = main_wa.width >> 1;
    MAIN_EMVADON = main_wa.width * main_wa.height;
    MAP_EMVADON = map_wa.width * map_wa.height;

    /* Init thread_ids dynamically */
    thread_ids = malloc(THREADS * 4);
    for (int i = 0; i < THREADS; i++)
        thread_ids[i] = i;

    FBSIZE = main_wa.width * main_wa.height * 4;

    /* Matrices initialization. */
    perspMat = perspectiveMatrix(FOV, ASPECTRATIO, ZNEAR, ZFAR);
    reperspMat = reperspectiveMatrix(FOV, ASPECTRATIO);
    orthoMat = orthographicMatrix(-100.f, 100.f, -100.f, 100.f, 0.01f, 1.f);

    AdjustShadow = 1;
    AdjustScene = 1;
}
const static void initAtoms(void) {

    wmatom[Win_Close] = XInternAtom(displ, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(displ, mainwin, &wmatom[Win_Close], 1);

    wmatom[Win_Name] = XInternAtom(displ, "WM_NAME", False);
    wmatom[Atom_Type] =  XInternAtom(displ, "STRING", False);
    XChangeProperty(displ, mainwin, wmatom[Win_Name], wmatom[Atom_Type], 8, PropModeReplace, (unsigned char*)"Anvil", 5);
}
/* Creates and Initializes the importand buffers. (frame, depth, shadow). */
const static void initBuffers(void) {
    frame_buffer = calloc(MAIN_EMVADON * 4, 1);
    map_buffer = calloc(MAP_EMVADON * 4, 1);

    main_depth_buffer = calloc(MAIN_EMVADON, 4);
    map_depth_buffer = calloc(MAP_EMVADON, 4);

    reset_buffer = calloc(MAIN_EMVADON * 4, 1);

    frags_buffer = calloc(MAIN_EMVADON, sizeof(Fragment));
    reset_frags = calloc(MAIN_EMVADON, sizeof(Fragment));

    shadow_buffer[0] = calloc(MAIN_EMVADON, 4);
    shadow_buffer[1] = calloc(MAIN_EMVADON, 4);
    shadow_buffer[2] = calloc(MAIN_EMVADON, 4);
}
const static void initLightModel(Light *l) {
    vec4f lightColor = { 1.0, 1.0, 1.0, 1.0 };
    Material mt = {
        .ambient = lightColor * AmbientStrength,
        .specular = lightColor * SpecularStrength,
        .diffuse = lightColor,
        .basecolor = lightColor
    };
    l->material = mt;
}
const static void pixmapcreate(void) {
    main_pixmap = XCreatePixmap(displ, mainwin, main_wa.width, main_wa.height, main_wa.depth);
    map_pixmap = XCreatePixmap(displ, mapwin, map_wa.width, map_wa.height, map_wa.depth);
}
const static void pixmapdisplay(Drawable pixmap, Drawable window, unsigned int width, unsigned int height) {
    XCopyArea(displ, pixmap, window, gc, 0, 0, width, height, 0, 0);
}
const static void announceReadyState(void) {
    printf("Announcing ready process state event\n");
    XEvent event = { 0 };
    event.xkey.type = KeyPress;
    event.xkey.keycode = 49;
    event.xkey.display = displ;

    /* Send the signal to our event dispatcher for further processing. */
    handler[event.type](&event);
}
const static void InitTimeCounter(void) {
    gettimeofday(&tv0, NULL);
    FramesPerFPS = 30;
}
const static void UpdateTimeCounter(void) {
    LastFrameTimeCounter = TimeCounter;
    gettimeofday(&tv, NULL);
    TimeCounter = (float)(tv.tv_sec - tv0.tv_sec) + 0.000001 * ((float)(tv.tv_usec - tv0.tv_usec));
    DeltaTime = TimeCounter - LastFrameTimeCounter;
}
const static void CalculateFPS(void) {
    Frame ++;

    if ( (Frame % FramesPerFPS) == 0 ) {
        FPS = ((float)(FramesPerFPS)) / (TimeCounter-prevTime);
        prevTime = TimeCounter;
    }
}
const static void displayInfo(void) {
    char info_string[30];
    sprintf(info_string, "Resolution: %d x %d", main_wa.width, main_wa.height);
    XDrawString(displ ,mainwin ,gc, 5, 12, info_string, strlen(info_string));

    sprintf(info_string, "Running Time: %4.1f", TimeCounter);
    XDrawString(displ ,mainwin ,gc, 5, 24, info_string, strlen(info_string));

    sprintf(info_string, "%4.1f fps", FPS);
    XDrawString(displ ,mainwin ,gc, 5, 36, info_string, strlen(info_string));
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
/* General initialization and event handling. */
const static int board(void) {

    if (!XInitThreads()) {
        fprintf(stderr, "Warning: board() -- XInitThreads()\n");
        return EXIT_FAILURE;
    }

    XEvent event;

    displ = XOpenDisplay(NULL);
    if (displ == NULL) {
        fprintf(stderr, "Warning: board() -- XOpenDisplay()\n");
        return EXIT_FAILURE;
    }

    initMainWindow();
    initMapWindow();
    InitTimeCounter();
    initGlobalGC();
    pixmapcreate();
    initAtoms();
    // registerSig(SIGSEGV);

    initDependedVariables();
    initBuffers();
    initLightModel(&sunlight);

    createScene(&scene);
    posWorldObjects(&scene);

    /* Announcing to event despatcher that starting initialization is done. We send a Keyress event to Despatcher to awake Projection. */
    announceReadyState();

    while (RUNNING) {

        // clock_t start_time = start();
        UpdateTimeCounter();
        CalculateFPS();
        displayInfo();
        project();
        // end(start_time);

        while(XPending(displ)) {

            XNextEvent(displ, &event);

            if (handler[event.type])
                handler[event.type](&event);
        }
        usleep(0);
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

    createMaterialDatabase();

    if (board()) {
        fprintf(stderr, "ERROR: main() -- board()\n");
        return EXIT_FAILURE;
    }

    XCloseDisplay(displ);
    
    return EXIT_SUCCESS;
}

