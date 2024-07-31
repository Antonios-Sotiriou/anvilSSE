/* General Macro definitions */
#define _GNU_SOURCE /* Importand to counter sigaction struct < incomplete type error >. */

/* general headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <sys/time.h>

/* Library to cooperate with Postgres Database. */
#include <postgresql/libpq-fe.h>

/* signal */
#include <signal.h>
// #include <immintrin.h>

/* testing */
#include "../headers/exec_time.h"
#include "../headers/logging.h"
#include "../headers/test_shapes.h"

/* ############################################## MULTITHREADING ################################################################### */
#include <pthread.h>
pthread_t threads[THREADS];
int *thread_ids;
/* ############################################## MULTITHREADING ################################################################### */

/* CHOOSE WITH WHICH FUNCTION TO RASTERIZE. */
int EDGEFUNC = 1;
int SCANLINE = 0;

/* Project specific headers */
#include "../headers/anvil_structs.h"
#include "../headers/quaternions.h"
#include "../headers/database.h"
#include "../headers/matrices.h"
#include "../headers/kinetics.h"
#include "../headers/scene_objects.h"
#include "../headers/general_functions.h"
#include "../headers/physics.h"
#include "../headers/clipping.h"
#include "../headers/graphics_pipeline.h"
#include "../headers/shadow_pipeline.h"
#include "../headers/edge_pipeline.h"
#include "../headers/terrain_functions.h"
#include "../headers/collision_detection.h"
#include "../headers/camera.h"
#include "../headers/draw_functions.h"
#include "../headers/click_select.h"

enum { Win_Close, Win_Name, Atom_Type, Atom_Last};

#define WIDTH                     1000
#define HEIGHT                    1000
#define POINTERMASKS              ( ButtonPressMask )
#define KEYBOARDMASKS             ( KeyPressMask | KeyReleaseMask )
#define EXPOSEMASKS               ( StructureNotifyMask )

/* X Global Structures. */
Display *displ;
Window mainwin;
XImage *main_image;
Pixmap main_pixmap;
GC gc;
XGCValues gcvalues;
XWindowAttributes main_wa, *point_attrib;
XSetWindowAttributes sa;
Atom wmatom[Atom_Last];

/* BUFFERS. */
vec4c *frame_buffer, *point_frame_buffer, *reset_frame_buffer;
float *main_depth_buffer, *point_depth_buffer, *reset_depth_buffer, *shadow_buffer[NUM_OF_CASCADES], *reset_shadow_buffer;
Fragment *frags_buffer, *reset_frags_buffer;

/* Project Global Variables. */
int PROJECTIONVIEW        = 0;
static int PROJECTBUFFER  = 1;
static int EYEPOINT       = 0;
static float FOV          = 45.0f;
static float ZNEAR        = 0.1f;
static float ZFAR         = 100.0f;
float ASPECTRATIO         = 1;
static int BSIZE          = 0;
float NPlane              = 1.0f;
float FPlane              = 100000.0f;
float AmbientStrength     = 0.5f;
float SpecularStrength    = 0.5f;
float DiffuseStrength     = 0.5f;
float shadow_bias         = 0.0f;//0.003105;//0.002138;//0.000487f;
int DISPLAYBBOX           = 0;
/* For investigating shadow map usefull global variables. */
int INCORDEC = -1;
unsigned int SMA = 0;
unsigned int SMB = 200;
unsigned int SMC = 400;
unsigned int SMD = 800;
unsigned int STA = 82;
unsigned int STB = 320;
unsigned int STC = 1280;
unsigned int STD = 5120;

/* Point of view. */
Mesh *eye;

vec4f gravity_epicenter = { 0.f, -1.f, 0.f };
const float sunMov = 100.0f;
const float movScalar = 100.f;
const float cameraMov = 1.42f;

/* Variables usefull for mesh click select. */
unsigned int getClick = 0;
int mesh_id = 0;
vec4f click = { 0 };

/* Global Matrices */
Mat4x4 perspMat, lookAt, viewMat, reperspMat, orthoMat, worldMat, ortholightMat[NUM_OF_CASCADES], persplightMat, *point_mat;

/* Anvil global Objects Meshes and Scene. */
Scene scene = { 0 };
/* Terrain info struct is populated with data when terrain is created(createTerrain()). */
TerrainInfo tf;
/* Screen tiles info structs pointer. */
Tile *tiles, *point_tiles;

/* X11 and mainwindow Global variables. */
static int INIT = 0;
static int RUNNING = 1;
// int HALFW = 0; // Half width of the screen; This variable is initialized in configurenotify function.Its Helping us decrease the number of divisions.
// int HALFH = 0; // Half height of the screen; This variable is initialized in configurenotify function.Its Helping us decrease the number of divisions.
vec4i half_screen = { 0, 0, 1, 1 };
int MAIN_EMVADON;
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

/* Self explanatory. Physics and collisions are computed whithin. */
const static void applyPhysics(void);

/* Represantation functions */
const static void project(void);
const static void drawFrame(void);

/* Xlib relative functions and event dispatcher. */
const static void initMainWindow(void);
const static void initGlobalGC(void);
const static void initDependedVariables(void);
const static void initAtoms(void);
const static void initBuffers(void);
// const static void initLightModel(Light *l);
const static void pixmapcreate(void);
const static void pixmapdisplay(Drawable pixmap, Drawable window, unsigned int wdth, unsigned int heigth);
// const static void initLightModel(Mesh *m);
const static void InitTimeCounter(void);
const static void UpdateTimeCounter(void);
const static void CalculateFPS(void);
const static void displayInfo(void);
const static void sigsegv_handler(const int sig);
const static int registerSig(const int signal);
static void *board(void *args);
static void (*handler[LASTEvent]) (XEvent *event) = {
    [ClientMessage] = clientmessage,
    [ReparentNotify] = reparentnotify,
    [MapNotify] = mapnotify,
    [ResizeRequest] = resizerequest,
    [ConfigureNotify] = configurenotify,
    [ButtonPress] = buttonpress,
    [KeyPress] = keypress,
    [KeyRelease] = keyrelease,
};

const static void clientmessage(XEvent *event) {
    printf("Received client message event\n");
    if (event->xclient.data.l[0] == wmatom[Win_Close]) {

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
    printf("configurenotify event received\n");
    if (!event->xconfigure.send_event) {
        printf("configurenotify Send event received\n");
        XGetWindowAttributes(displ, mainwin, &main_wa);

        if (INIT) {

            free(frame_buffer);
            free(reset_frame_buffer);

            free(main_depth_buffer);
            free(reset_depth_buffer);

            free(frags_buffer);
            free(reset_frags_buffer);

            free(shadow_buffer[0]);
            free(shadow_buffer[1]);
            free(shadow_buffer[2]);
            free(shadow_buffer[3]);
            free(reset_shadow_buffer);

            free(main_image);

            free(tiles);

            initDependedVariables();

            initBuffers();
            pixmapcreate();
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
}
const static void keypress(XEvent *event) {

    KeySym keysym = XLookupKeysym(&event->xkey, 0);

    // printf("\x1b[H\x1b[J");
    // system("clear\n");
    printf("Key Pressed: %ld\n", keysym);
    // logEvent(*event);

    switch (keysym) {
        case 65505 : INCORDEC = INCORDEC == -1 ? 1 : -1; break; /* control */
        case 49 : SMA += INCORDEC; break;
        case 50 : SMB += INCORDEC; break;
        case 51 : SMC += INCORDEC; break;
        case 52 : SMD += INCORDEC; break;
        case 48 : STA += INCORDEC; break;
        case 57 : STB += INCORDEC; break;
        case 56 : STC += INCORDEC; break;
        case 55 : STD += INCORDEC; break;
        case 98 : DISPLAYBBOX = DISPLAYBBOX == 0 ? 1 : 0; break;  /* b */
        case 97 : look_left(&scene.m[6], 2.0);             /* a */
            break;
        case 100 : look_right(&scene.m[6], 2.0);           /* d */
            break;
        case 113 : look_up(&scene.m[6], 2.0);              /* q */
            break;
        case 101 : look_down(&scene.m[6], 2.0);            /* e */
            break;
        case 119 : move_forward(&scene.m[6], cameraMov);         /* w */
            break;
        case 115 : move_backward(&scene.m[6], cameraMov);        /* s */
            break;
        case 65361 : move_left(&scene.m[6], cameraMov);          /* left arrow */
            break;
        case 65363 : move_right(&scene.m[6], cameraMov);         /* right arrow */
            break;
        case 65362 : move_up(&scene.m[6], cameraMov);            /* up arror */
            break;
        case 65364 : move_down(&scene.m[6], cameraMov);          /* down arrow */
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
            scene.m[1].momentum = movScalar * DeltaTime;
            scene.m[1].roll = 1;
            break;
        case 65432 : //sunlight.pos[0] += sunMov;                   /* Adjust Light Source */
            // vec4f mvb = -norm_vec(camera[N] - camera[U]);
            vec4f mvb = { 1.f, 0.f, 0.f };
            scene.m[1].mvdir = mvb;
            scene.m[1].momentum = movScalar * DeltaTime;
            scene.m[1].roll = 1;
            break;
        case 65434 : //sunlight.pos[1] += sunMov;                   /* Adjust Light Source */
            scene.m[1].grounded = 0;
            vec4f mvc = { 0.f, 1.f, 0.f };
            scene.m[1].mvdir = mvc;
            scene.m[1].momentum = 10 * scene.m[1].mass;
            scene.m[1].falling_time = 0.f;
            break;
        case 65435 : //sunlight.pos[1] -= sunMov;                   /* Adjust Light Source */
            vec4f mvd = { 0.f, -1.f, 0.f };
            scene.m[1].mvdir = mvd;
            scene.m[1].momentum = movScalar * DeltaTime;
            break;
        case 65431 : //sunlight.pos[2] += sunMov;                   /* Adjust Light Source */
            // vec4f mve = norm_vec(scene.m[Camera_1].cd.v[U] + scene.m[Camera_1].cd.v[N]);
            vec4f mve = { 0.f, 0.f, 1.f };
            scene.m[1].mvdir = mve;
            scene.m[1].momentum = movScalar * DeltaTime;
            scene.m[1].roll = 1;
            break;
        case 65433 : //sunlight.pos[2] -= sunMov;                   /* Adjust Light Source */
            // vec4f mvf = -norm_vec(scene.m[Camera_1].cd.v[U] + scene.m[Camera_1].cd.v[N]);
            vec4f mvf = { 0.f, 0.f, -1.f };
            scene.m[1].mvdir = mvf;
            scene.m[1].momentum = movScalar * DeltaTime;
            scene.m[1].roll = 1;
            break;
        case 120 : rotate_x(&scene.m[0], 1);                     /* x */
            break;
        case 121 :
            // rotate_y(&scene.m[0], 10);                           /* y */
            rotate_y(&scene.m[1], 10);
            break;
        case 122 : rotate_z(&scene.m[0], 1);                     /* z */
            break;
        case 114 : 
            vec4f center = { 0.f, 0.f, 0.f, 0.f };
            // rotate_light(&sunlight, center, 1, 1.0f, 0.0f, 0.0f);   /* r */
            break;
        case 99 :                                                        /* c */
            // rotate_origin(&scene.m[0], 10, 1.0f, 0.0f, 0.0f);
            if ( mesh_id > 0 )
                rotate_origin(&scene.m[mesh_id], 10, 1.0f, 0.0f, 0.0f);
            break;
        case 43 : AmbientStrength += 0.01;                                    /* + */
            printf("AmbientStrength: %f\n", AmbientStrength);
            break;
        case 45 : AmbientStrength -= 0.01;                                   /* - */
            printf("AmbientStrength: %f\n", AmbientStrength);
            break;
        case 112 :
            printf("keycode: %d\n", event->xkey.keycode);
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

    // applyForces(&scene);
}
const static void keyrelease(XEvent *event) {

    KeySym keysym = XLookupKeysym(&event->xkey, 0);

    printf("Key Released: %ld\n", keysym);
    eye->momentum = 0;
    eye->rot_angle = 0;
    if ( keysym == 99 )
        scene.m[mesh_id].rot_angle = 0;
}
static void *cascade(void *args) {

    int shadow_id = *(int*)args;

    memcpy(shadow_buffer[shadow_id], reset_shadow_buffer, BSIZE);
    shadowPipeline(&scene, shadow_id);

    return (void*)args;
}
static void *fragmentShader(void *args) {

    int thread_id = *(int*)args;

    int ypol = ( (MAIN_EMVADON) % 6);
    int stuck = ( (MAIN_EMVADON) / 6);

    int tile_size = stuck * (thread_id + 1);
    int step = stuck * thread_id;

    if ( ypol && (thread_id == 5) )
        tile_size += ypol;

    for (int i = step; i < tile_size; i++) {
        if ( frags_buffer[i].state )
            phong(&frags_buffer[i]);
    }

    return (void*)args;
}
const static void applyPhysics(void) {

    applyForces(&scene);

    // printQuad(scene.m[Player_1].quadIndex);

    // displayVec4f(scene.m[Player_1].pivot, scene.m[Player_1].pivot + (scene.m[Player_1].mvdir * scene.m[Player_1].momentum), worldMat);
}
const static void project(void) {


    if (EYEPOINT)
        eye = &scene.m[7];
    else
        eye = &scene.m[6];

    lookAt = lookat(eye->cd.v[P], eye->cd.v[U], eye->cd.v[V], eye->cd.v[N]);
    viewMat = inverse_mat(lookAt);

    // printf("SMA: %d    SMB: %d    SMC: %d    SMD: %d    INCORDEC: %d\n", SMA, SMB, SMC, SMD, INCORDEC);
    // printf("STA: %d    STB: %d    STC: %d    STD: %d    INCORDEC: %d\n", STA, STB, STC, STD, INCORDEC);
    createCascadeShadowMatrices();

    if (!PROJECTIONVIEW)
        worldMat = mxm(viewMat, perspMat);
    else
        worldMat = mxm(viewMat, orthoMat);

    // clock_t time_0 = start();
    /* Draw in parallel the 4 Cascade shadow maps. */
    for (int i = 0; i < 4; i++) {
        if (pthread_create(&threads[i], NULL, &cascade, &thread_ids[i]))
            fprintf(stderr, "ERROR: project() -- graphicsPipeline -- pthread_create()\n");
    }

    for (int i = 0; i < 4; i++) {
        if (pthread_join(threads[i], NULL))
            fprintf(stderr, "ERROR: project() -- cascade -- pthread_join()\n");
    }
    // printf("Shadow Pipeline  : %f\n", end(time_0));

    clock_t time_1 = start();
    graphicsPipeline(NULL);
    printf("Graphics Pipeline: %f\n", end(time_1));

    // clock_t time_2 = start();
    /* Proceed the fragments buffer created by grafikPipeline and apply lighting. */
    for (int i = 0; i < 6; i++) {
        if (pthread_create(&threads[i], NULL, &fragmentShader, &thread_ids[i]))
            fprintf(stderr, "ERROR: project() -- oscillator -- pthread_create()\n");
    }
    for (int i = 0; i < 6; i++) {
        if (pthread_join(threads[i], NULL))
            fprintf(stderr, "ERROR: project() -- oscillator -- pthread_join()\n");
    }
    // printf("Fragment Shader  : %f\n", end(time_2));

    if ( getClick ) {
        mesh_id = clickSelect(frags_buffer[((int)click[1] * main_wa.width) + (int)click[0]].pos);
        getClick = 0;
    }

    if ( DISPLAYBBOX )
        displayBbox(scene.m[mesh_id].bbox.v, scene.m[mesh_id].bbox.v_indexes, worldMat);

    drawFrame();
}
/* Writes the final Pixel values on screen. */
const static void drawFrame(void) {
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
    else if (PROJECTBUFFER == 6)
        main_image->data = (char*)shadow_buffer[3];

    XPutImage(displ, main_pixmap, gc, main_image, 0, 0, 0, 0, main_wa.width, main_wa.height);
    pixmapdisplay(main_pixmap, mainwin, main_wa.width, main_wa.height);

    memcpy(frame_buffer, reset_frame_buffer, BSIZE);
    memcpy(main_depth_buffer, reset_depth_buffer, BSIZE);
    memcpy(frags_buffer, reset_frags_buffer, MAIN_EMVADON * sizeof(Fragment));
}
const static void initMainWindow(void) {
    sa.event_mask = EXPOSEMASKS | KEYBOARDMASKS | POINTERMASKS;
    sa.background_pixel = 0x000000;
    mainwin = XCreateWindow(displ, XRootWindow(displ, XDefaultScreen(displ)), 0, 0, WIDTH, HEIGHT, 0, CopyFromParent, InputOutput, CopyFromParent, CWBackPixel | CWEventMask, &sa);
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
    main_image = XCreateImage(displ, main_wa.visual, main_wa.depth, ZPixmap, 0, (char*)point_frame_buffer, main_wa.width, main_wa.height, 32, (main_wa.width * 4));

    ASPECTRATIO = ((float)main_wa.width / (float)main_wa.height);

    half_screen[0] = main_wa.height >> 1;
    half_screen[1] = main_wa.width >> 1;
    MAIN_EMVADON = main_wa.width * main_wa.height;

    /* Init tiles dynamically  ######################################################################## */
    tiles = calloc(6, sizeof(Tile));
    int tileHeightYpol = main_wa.height % 6;
    int tileHeight = main_wa.height / 6;

    for (int i = 0; i < 6; i++) {
        tiles[i].start_width = 0;
        tiles[i].end_width = main_wa.width;
        tiles[i].start_height += tileHeight * i;
        tiles[i].end_height += tileHeight * (i + 1);

        if ( (i == 5) && (tileHeightYpol) ) {
            tiles[i].end_height += tileHeightYpol;
        }
    }
    /* ################################################################################################ */

    BSIZE = main_wa.width * main_wa.height * 4;

    /* Matrices initialization. */
    perspMat = perspectiveMatrix(FOV, ASPECTRATIO, ZNEAR, ZFAR);
    reperspMat = reperspectiveMatrix(FOV, ASPECTRATIO);
    orthoMat = orthographicMatrix(-100.f, 100.f, -100.f, 100.f, 0.01f, 1.f);
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
    frame_buffer = calloc(MAIN_EMVADON, 4);
    reset_frame_buffer = calloc(MAIN_EMVADON, 4);

    main_depth_buffer = calloc(MAIN_EMVADON, 4);
    reset_depth_buffer = calloc(MAIN_EMVADON, 4);

    frags_buffer = calloc(MAIN_EMVADON, sizeof(Fragment));
    reset_frags_buffer = calloc(MAIN_EMVADON, sizeof(Fragment));

    shadow_buffer[0] = calloc(MAIN_EMVADON, 4);
    shadow_buffer[1] = calloc(MAIN_EMVADON, 4);
    shadow_buffer[2] = calloc(MAIN_EMVADON, 4);
    shadow_buffer[3] = calloc(MAIN_EMVADON, 4);

    reset_shadow_buffer = malloc(MAIN_EMVADON * 4);
    for (int i = 0; i < MAIN_EMVADON; i++)
        reset_shadow_buffer[i] = 100000.f;
}
// const static void initLightModel(Mesh *m) {
//     vec4f lightColor = { 1.0, 1.0, 1.0, 1.0 };
//     Material mt = {
//         .ambient = lightColor * AmbientStrength,
//         .specular = lightColor * SpecularStrength,
//         .diffuse = lightColor,
//         .basecolor = lightColor
//     };
//     m->material = mt;
// }
const static void pixmapcreate(void) {
    main_pixmap = XCreatePixmap(displ, mainwin, main_wa.width, main_wa.height, main_wa.depth);
}
const static void pixmapdisplay(Drawable pixmap, Drawable window, unsigned int width, unsigned int height) {
    XCopyArea(displ, pixmap, window, gc, 0, 0, width, height, 0, 0);
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
    FPS = Frame / TimeCounter;
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
static void *board(void *args) {
    int th_id = *(int*)args;

    if ( th_id == 7 ) {
        initGlobalGC();
        pixmapcreate();
        initAtoms();
        // registerSig(SIGSEGV);

        initDependedVariables();
        initBuffers();

        createScene(&scene);     /*  Scene creation must happen after world objects initialization.    */
        initWorldObjects(&scene);

        // initLightModel(&scene.m[Light_1]);

        // initTerrainInfo(&tf);

        float time_dif;
        while(RUNNING) {

            UpdateTimeCounter();
            CalculateFPS();
            displayInfo();
            // clock_t start_time = start();
            applyPhysics();
            // end(start_time);
            // clock_t start_time = start();
            project();
            // end(start_time);

            time_dif = DeltaTime > 0.016666 ? 0 : (0.016666 - DeltaTime) * 100000;
            usleep(time_dif);
        }
    } else {
        XEvent event;
        int is_repeated = 0;
        while(RUNNING) {

            XNextEvent(displ, &event);
            if ( event.type == KeyRelease ) {
                XEvent cache_1 = { 0 };

                usleep(3300);
                if ( XEventsQueued(displ, QueuedAfterReading) ) {

                    XPeekEvent(displ, &cache_1);
                    if ( cache_1.type == KeyPress && (cache_1.xkey.serial == event.xkey.serial) && (cache_1.xkey.time == event.xkey.time) ) {
                        XNextEvent(displ, &event);
                        is_repeated = 1;
                    }
                }
            }

            // if ( is_repeated ) {
            //     is_repeated = 0;
            //     continue;
            // }

            if (handler[event.type])
                handler[event.type](&event);

            usleep(3300);
        }
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

    InitTimeCounter();

    createMaterialDatabase();

    initThreads();

    for (int i = 7; i > 5; i--)
        pthread_create(&threads[i], NULL, &board, &thread_ids[i]);

    for (int i = 7; i > 5; i--)
        pthread_join(threads[i], NULL);

    free(frame_buffer);
    free(reset_frame_buffer);

    free(main_depth_buffer);
    free(reset_depth_buffer);

    free(frags_buffer);
    free(reset_frags_buffer);

    free(shadow_buffer[0]);
    free(shadow_buffer[1]);
    free(shadow_buffer[2]);
    free(shadow_buffer[3]);
    free(reset_shadow_buffer);

    for (int i = 0; i < tf.quadsArea; i++) {
        if (tf.quads[i].members)
            free(tf.quads[i].members);
    }
    free(tf.quads);

    free(tiles);

    releaseScene(&scene);

    free(main_image);
    XFreeGC(displ, gc);
    XFreePixmap(displ, main_pixmap);
    XDestroyWindow(displ, mainwin);

    free(thread_ids);
    XCloseDisplay(displ);
    
    return EXIT_SUCCESS;
}



