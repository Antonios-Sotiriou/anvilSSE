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

int EDGEFUNC = 0;
int SCANLINE = 1;

/* Project specific headers */
#include "headers/locale.h"
#include "headers/anvil_structs.h"
#include "headers/matrices.h"
#include "headers/kinetics.h"
#include "headers/clipping.h"
#include "headers/grfk_pipeline.h"
#include "headers/camera.h"
#include "headers/world_objects.h"
#include "headers/general_functions.h"
#include "headers/shadowmap.h"
#include "headers/draw_functions.h"

/* testing */
#include "headers/exec_time.h"
#include "headers/logging.h"
#include "headers/test_shapes.h"

enum { Win_Close, Win_Name, Atom_Type, Atom_Last};
enum { Pos, U, V, N, C, newPos };

#define WIDTH                     1000
#define HEIGHT                    1000
#define POINTERMASKS              ( ButtonPressMask )
#define KEYBOARDMASKS             ( KeyPressMask )
#define EXPOSEMASKS               ( StructureNotifyMask )

/* X Global Structures. */
Display *displ;
Window win;
XImage *image;
Pixmap pixmap;
GC gc;
XGCValues gcvalues;
XWindowAttributes wa;
XSetWindowAttributes sa;
Atom wmatom[Atom_Last];

/* BUFFERS. */
u_int8_t *frame_buffer;
float *depth_buffer, *shadow_buffer;

/* Project Global Variables. */
static int PROJECTIONVIEW = 0;
static int PROJECTBUFFER  = 1;
static int AdjustShadow   = 1;
static int AdjustScene    = 1;
static int EYEPOINT       = 0;
static float FOV          = 45.0f;
static float ZNEAR        = 0.01f;
static float ZFAR         = 1000.0f;
static float ASPECTRATIO  = 1;
static int FBSIZE         = 0;
float NPlane              = 1.0f;
float FPlane              = 0.00001f;
float SCALE               = 0.003f;
float AmbientStrength     = 0.15f;
float SpecularStrength    = 0.5f;
float shadow_bias         = 0.0f;

/* Camera and Global light Source. */
vec4f camera[N + 1] = {
    { 0.0f, 0.0f, -10.0f, 1.0f },
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f }
};
Light sunlight = {
    // .pos = { -800.001343f, 205.598007f, 802.004822f, 1.000000f },
    // .u = { -0.694661f, 0.000000f, -0.719352f, 0.000000f },
    // .v = { 0.000000f, -1.000000f, 0.000000f, 0.000000f },
    // .n = { 0.719352f, 0.000000f, -0.694661f, 0.000000f },
    .pos = { 0.f, 0.0f, 0.f, 1.f },
    .u = { 1.f, 0.f, 0.f, 0.f },
    .v = { 0.f, 0.f, -1.f, 0.f },
    .n = { 0.f, -1.f, 0.f, 0.f },
};

/* Global Matrices */
Mat4x4 perspMat, lookAt, viewMat, reperspMat, orthoMat, worldMat, lightMat;

/* Anvil global Objects Meshes and Scene. */
Scene scene = { 0 };

/* X11 and window Global variables. */
static int INIT = 0;
static int RUNNING = 1;
int HALFW = 0; // Half width of the screen; This variable is initialized in configurenotify function.Its Helping us decrease the number of divisions.
int HALFH = 0; // Half height of the screen; This variable is initialized in configurenotify function.Its Helping us decrease the number of divisions.
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
const static void pixmapdisplay(void);
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

const static void clientmessage(XEvent *event) {
    printf("Received client message event\n");
    if (event->xclient.data.l[0] == wmatom[Win_Close]) {

        releaseScene(&scene);

        free(frame_buffer);
        free(depth_buffer);
        free(shadow_buffer);

        free(image);
        XFreeGC(displ, gc);
        XFreePixmap(displ, pixmap);
        XDestroyWindow(displ, win);

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
        // int old_height = wa.height;
        XGetWindowAttributes(displ, win, &wa);

        if (INIT) {
            free(frame_buffer);
            free(depth_buffer);
            free(shadow_buffer);

            free(image);
            initBuffers();
            pixmapcreate();

            initDependedVariables();
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

    vec4f *eye;
    if (EYEPOINT)
        eye = (vec4f*)&sunlight;
    else
        eye = (vec4f*)&camera;

    printf("Key Pressed: %ld\n", keysym);
    // printf("\x1b[H\x1b[J");
    system("clear\n");
    switch (keysym) {
        case 97 : look_left(eye, 0.2);       /* a */
            break;
        case 100 : look_right(eye, 0.2);     /* d */
            break;
        case 113 : look_up(eye, 2.2);       /* q */
            break;
        case 101 : look_down(eye, 2.2);     /* e */
            break;
        case 119 : move_forward(eye, 2.2);         /* w */
            break;
        case 115 : move_backward(eye, 2.2);        /* s */
            break;
        case 65361 : move_left(eye, 2.2);          /* left arrow */
            break;
        case 65363 : move_right(eye, 2.2);         /* right arrow */
            break;
        case 65362 : move_up(eye, 0.2);            /* up arror */
            break;
        case 65364 : move_down(eye, 0.2);          /* down arrow */
            break;
        case 65451 :shadow_bias += 0.0001;             /* + */
            printf("shadow_bias: %f\n",shadow_bias);
            break;
        case 65453 :shadow_bias -= 0.0001;             /* - */
            printf("shadow_bias: %f\n", shadow_bias);
            break;
        case 65450 : NPlane += 0.01;             /* * */
            printf("NPlane: %f\n", NPlane);
            break;
        case 65455 : NPlane -= 0.01;             /* / */
            printf("NPlane: %f\n", NPlane);
            break;
        case 65430 : sunlight.pos[0] -= 10.1;                   /* Adjust Light Source */
            break;
        case 65432 : sunlight.pos[0] += 10.1;                   /* Adjust Light Source */
            break;
        case 65431 : sunlight.pos[2] += 10.1;                   /* Adjust Light Source */
            break;
        case 65433 : sunlight.pos[2] -= 10.1;                   /* Adjust Light Source */
            break;
        case 65434 : sunlight.pos[1] += 10.1;                   /* Adjust Light Source */
            break;
        case 65435 : sunlight.pos[1] -= 10.1;                   /* Adjust Light Source */
            break;
        case 120 : rotate_x(&scene.m[0], 1);                     /* x */
            break;
        case 121 : rotate_y(&scene.m[0], 1);                     /* y */
            break;
        case 122 : rotate_z(&scene.m[0], 1);                     /* z */
            break;
        case 114 : rotate_light(&sunlight, 1, 0.0, 1.0, 0.0);        /* r */
            break;
        case 99 : rotate_origin(&scene.m[1], 1, 1.0, 0.0, 0.0);  /* c */
            break;
        case 43 : SCALE += 0.01;                                    /* + */
            orthoMat = orthographicMatrix(SCALE, SCALE, 0.0f, 0.0f, ZNEAR, ZFAR);
            break;
        case 45 : SCALE -= 0.01;                                   /* - */
            orthoMat = orthographicMatrix(SCALE, SCALE, 0.0f, 0.0f, ZNEAR, ZFAR);
            break;
        case 112 :
            if (PROJECTBUFFER == 3)
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
    AdjustShadow = 1;
    AdjustScene = 1;

    if (!PROJECTIONVIEW)
        worldMat = mxm(viewMat, perspMat);
    else
        worldMat = mxm(viewMat, orthoMat);
    // logMatrix(worldMat);
    // project();
    // logVec4f(eye[Pos]);
    // logVec4f(eye[U]);
    // logVec4f(eye[V]);
    // logVec4f(eye[N]);
}
const static void project() {
    if (AdjustShadow) {
        memset(shadow_buffer, '@', FBSIZE);
        shadowPipeline(scene);
        AdjustShadow = 0;
    }
    if (AdjustScene) {
        memset(frame_buffer, 0, FBSIZE);
        memset(depth_buffer, 0, FBSIZE);
        grfkPipeline(scene);
        AdjustScene = 0;
    }
    drawFrame();
}
/* Writes the final Pixel values on screen. */
const static void drawFrame(void) {
    if (PROJECTBUFFER <= 1)
        image->data = (char*)frame_buffer;
    else if (PROJECTBUFFER == 2)
        image->data = (char*)depth_buffer;
    else if (PROJECTBUFFER == 3)
        image->data = (char*)shadow_buffer;

    XPutImage(displ, pixmap, gc, image, 0, 0, 0, 0, wa.width, wa.height);

    // memset(frame_buffer, '@', FBSIZE);
    // memset(depth_buffer, 0, FBSIZE);
    pixmapdisplay();
}
const static void initMainWindow(void) {
    sa.event_mask = EXPOSEMASKS | KEYBOARDMASKS | POINTERMASKS;
    sa.background_pixel = 0x000000;
    win = XCreateWindow(displ, XRootWindow(displ, XDefaultScreen(displ)), 0, 0, WIDTH, HEIGHT, 0, CopyFromParent, InputOutput, CopyFromParent, CWBackPixel | CWEventMask, &sa);
    XMapWindow(displ, win);
    XGetWindowAttributes(displ, win, &wa);
}
const static void initGlobalGC(void) {
    gcvalues.foreground = 0xffffff;
    gcvalues.background = 0x000000;
    gcvalues.graphics_exposures = False;
    gc = XCreateGC(displ, win, GCBackground | GCForeground | GCGraphicsExposures, &gcvalues);
}
const static void initDependedVariables(void) {
    image = XCreateImage(displ, wa.visual, wa.depth, ZPixmap, 0, (char*)frame_buffer, wa.width, wa.height, 32, (wa.width * 4));

    ASPECTRATIO = ((float)wa.width / (float)wa.height);
    HALFH = wa.height >> 1;
    HALFW = wa.width >> 1;

    FBSIZE = wa.width * wa.height * 4;

    /* Matrices initialization. */
    perspMat = perspectiveMatrix(FOV, ASPECTRATIO, ZNEAR, ZFAR);
    reperspMat = reperspectiveMatrix(FOV, ASPECTRATIO);
    orthoMat = orthographicMatrix(SCALE, SCALE, 0.f, 0.0, ZNEAR, ZFAR);

    AdjustShadow = 1;
    AdjustScene = 1;
}
const static void initAtoms(void) {

    wmatom[Win_Close] = XInternAtom(displ, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(displ, win, &wmatom[Win_Close], 1);

    wmatom[Win_Name] = XInternAtom(displ, "WM_NAME", False);
    wmatom[Atom_Type] =  XInternAtom(displ, "STRING", False);
    XChangeProperty(displ, win, wmatom[Win_Name], wmatom[Atom_Type], 8, PropModeReplace, (unsigned char*)"Anvil", 5);
}
/* Creates and Initializes the importand buffers. (frame, depth, shadow). */
const static void initBuffers(void) {
    frame_buffer = calloc(wa.width * wa.height * 4, 1);
    depth_buffer = calloc(wa.width * wa.height, 4);
    shadow_buffer = calloc(wa.width * wa.height, 4);
}
const static void initLightModel(Light *l) {
    vec4f lightColor = { 1.0, 1.0, 1.0, 1.0 };
    Material mt = {
        .ambient = lightColor * AmbientStrength,
        .specular = lightColor,
        .diffuse = lightColor,
        .basecolor = lightColor
    };
    l->material = mt;
}
const static void pixmapcreate(void) {
    pixmap = XCreatePixmap(displ, win, wa.width, wa.height, wa.depth);
}
const static void pixmapdisplay(void) {
    XCopyArea(displ, pixmap, win, gc, 0, 0, wa.width, wa.height, 0, 0);
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
    sprintf(info_string, "Resolution: %d x %d", wa.width, wa.height);
    XDrawString(displ ,win ,gc, 5, 12, info_string, strlen(info_string));

    sprintf(info_string, "Running Time: %4.1f", TimeCounter);
    XDrawString(displ ,win ,gc, 5, 24, info_string, strlen(info_string));

    sprintf(info_string, "%4.1f fps", FPS);
    XDrawString(displ ,win ,gc, 5, 36, info_string, strlen(info_string));
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

    if (board()) {
        fprintf(stderr, "ERROR: main() -- board()\n");
        return EXIT_FAILURE;
    }

    XCloseDisplay(displ);
    
    return EXIT_SUCCESS;
}

