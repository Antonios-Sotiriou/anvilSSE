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

/* Project specific headers */
#include "headers/locale.h"
#include "headers/anvil_structs.h"
#include "headers/matrices.h"
#include "headers/kinetics.h"
#include "headers/grfk_pipeline.h"
#include "headers/camera.h"
#include "headers/world_objects.h"
#include "headers/general_functions.h"

/* testing */
#include "headers/exec_time.h"
#include "headers/logging.h"
#include "headers/test_shapes.h"

enum { Win_Close, Win_Name, Atom_Type, Atom_Last};
enum { Pos, U, V, N, C };

#define WIDTH                     1000
#define HEIGHT                    1000
#define POINTERMASKS              ( ButtonPressMask )
#define KEYBOARDMASKS             ( KeyPressMask )
#define EXPOSEMASKS               ( StructureNotifyMask )

/* X Global Structures. */
Display *displ;
Window win;
Pixmap pixmap;
GC gc;
XGCValues gcvalues;
XWindowAttributes wa;
XSetWindowAttributes sa;
Atom wmatom[Atom_Last];

/* BUFFERS. */
u_int8_t *frame_buffer;

/* Project Global Variables. */
static float FOV          =  45.0;
static float ZNEAR        =  0.01;
static float ZFAR         =  1000.0;
static float ASPECTRATIO  = 1;
static int EYEPOINT = 0;

/* Camera and Global light Source. */
vec4f camera[N + 1] = {
    { 0.0, 0.0, 0.0, 1.0 },
    { 1.0, 0.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0, 0.0 }
};
vec4f light[C + 1] = {
    { -56.215076, -47.867058, 670.036438, 1.0 },
    { -0.907780, -0.069064, -0.413726, 0.0 },
    { -0.178108, 0.956481, 0.231131, 0.0 },
    { 0.379759, 0.283504, -0.880576, 0.0 },
    { 1.0, 1.0, 1.0}
};

/* Global Matrices */
Mat4x4 perspMat, LookAt, ViewMat;

/* Anvil global Objects Meshes and Scene. */
Scene scene = { 0 };

/* X11 and window Global variables. */
static int INIT = 0;
static int RUNNING = 1;
int HALFW = 0; // Half width of the screen; This variable is initialized in configurenotify function.Its Helping us decrease the number of divisions.
int HALFH = 0; // Half height of the screen; This variable is initialized in configurenotify function.Its Helping us decrease the number of divisions.
static int DEBUG = 0;

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
const static KeySym getKeysym(XEvent *event);
const static void initMainWindow();
const static void initGlobalGC();
const static void initDependedVariables(void);
const static void pixmapcreate(void);
const static void pixmapdisplay(void);
const static void atomsinit(void);
const static void sigsegv_handler(const int sig);
const static int registerSig(const int signal);
const static void initBuffers(void);
static int board(void);
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
        int old_height = wa.height;
        XGetWindowAttributes(displ, win, &wa);

        if (INIT) {
            free(frame_buffer);
            initBuffers();
            pixmapcreate();

            initDependedVariables();
        }

        if (!INIT) {
            LookAt = lookat(camera[Pos], camera[U], camera[V], camera[N]);
            ViewMat = inverse_mat(LookAt);
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
    
    KeySym keysym = getKeysym(event);

    vec4f *eye;
    if (EYEPOINT)
        eye = &light[0];
    else
        eye = &camera[0];

    printf("Key Pressed: %ld\n", keysym);
    printf("\x1b[H\x1b[J");
    switch (keysym) {
        case 119 : move_forward(eye);         /* w */
            break;
        case 115 : move_backward(eye);        /* s */
            break;
        case 65361 : move_left(eye);          /* left arrow */
            break;
        case 65363 : move_right(eye);         /* right arrow */
            break;
        case 65362 : move_up(eye);            /* up arror */
            break;
        case 65364 : move_down(eye);          /* down arrow */
            break;
        // case 120 : rotate_x(&scene.m[1], 1);                     /* x */
        //     break;
        case 121 : rotate_y(&scene.m[0], 1);                     /* y */
            break;
        // case 122 : rotate_z(&scene.m[0], 1);                     /* z */
        //     break;
        // case 114 : rotate_light(light, 1, 0.0, 1.0, 0.0);        /* r */
        //     break;
        // case 99 : rotate_origin(&scene.m[2], 1, 1.0, 0.0, 0.0);  /* c */
        //     break;
    }
    LookAt = lookat(eye[Pos], eye[U], eye[V], eye[N]);
    ViewMat = inverse_mat(LookAt);
    // project();
}
// ##############################################################################################################################################
/* Starts the Projection Pipeline. */ // ########################################################################################################
const static void project() {

    Mesh cache;
    initMesh(&cache, scene.m[0]);
    Mat4x4 WorldMat = mxm(ViewMat, perspMat);

    cache.v = meshxm(scene.m[0].v, scene.m[0].v_indexes, WorldMat);
    ppdiv(cache.v, cache.v_indexes);
    cache = bfculling(cache);
    viewtoscreen(cache.v, cache.v_indexes);

    drawFrame();

    for (int i = 0; i < cache.f_indexes; i++) {
        XDrawLine(displ, win, gc, cache.v[cache.f[i].a[0]][0], cache.v[cache.f[i].a[0]][1], cache.v[cache.f[i].b[0]][0], cache.v[cache.f[i].b[0]][1]);
        XDrawLine(displ, win, gc, cache.v[cache.f[i].b[0]][0], cache.v[cache.f[i].b[0]][1], cache.v[cache.f[i].c[0]][0], cache.v[cache.f[i].c[0]][1]);
        XDrawLine(displ, win, gc, cache.v[cache.f[i].c[0]][0], cache.v[cache.f[i].c[0]][1], cache.v[cache.f[i].a[0]][0], cache.v[cache.f[i].a[0]][1]);
    }

    releaseMesh(&cache);
}
/* Writes the final Pixel values on screen. */
const static void drawFrame(void) {

    XImage *image = XCreateImage(displ, wa.visual, wa.depth, ZPixmap, 0, frame_buffer, wa.width, wa.height, 32, (wa.width * 4));
    XPutImage(displ, pixmap, gc, image, 0, 0, 0, 0, wa.width, wa.height);
    free(image);

    pixmapdisplay();
}
/* Starts the Projection Pipeline. */ // ########################################################################################################
// ##############################################################################################################################################
const static KeySym getKeysym(XEvent *event) {

    /* Get Keyboard UTF-8 input */
    XIM xim = { 0 };
    xim = XOpenIM(displ, NULL, NULL, NULL);
    if (xim == NULL) {
        perror("keypress() - XOpenIM()");
    }

    XIC xic = { 0 };
    xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, win, NULL);
    if (xic == NULL) {
        perror("keypress() - XreateIC()");
    }
    XSetICFocus(xic);

    KeySym keysym = 0;
    char buffer[32];
    Status status = 0;
    Xutf8LookupString(xic, &event->xkey, buffer, 32, &keysym, &status);
    if (status == XBufferOverflow) {
        perror("Buffer Overflow...\n");
    }
    return keysym;
}
const static void initMainWindow(void) {
    sa.event_mask = EXPOSEMASKS | KEYBOARDMASKS | POINTERMASKS;
    sa.background_pixel = 0x000000;
    win = XCreateWindow(displ, XRootWindow(displ, XDefaultScreen(displ)), 0, 0, WIDTH, HEIGHT, 0, CopyFromParent, InputOutput, CopyFromParent, CWBackPixel | CWEventMask, &sa);
    XMapWindow(displ, win);
    XGetWindowAttributes(displ, win, &wa);
}
void InitTimeCounter() {
    gettimeofday(&tv0, NULL);
    FramesPerFPS = 30;
}
void UpdateTimeCounter() {
    LastFrameTimeCounter = TimeCounter;
    gettimeofday(&tv, NULL);
    TimeCounter = (float)(tv.tv_sec - tv0.tv_sec) + 0.000001 * ((float)(tv.tv_usec - tv0.tv_usec));
    DeltaTime = TimeCounter - LastFrameTimeCounter;
}
void CalculateFPS() {
    Frame ++;

    if ( (Frame % FramesPerFPS) == 0 ) {
        FPS = ((float)(FramesPerFPS)) / (TimeCounter-prevTime);
        prevTime = TimeCounter;
    }
}
void displayInfo() {
    char info_string[30];
    sprintf(info_string, "Resolution: %d x %d\0", wa.width, wa.height);
    XDrawString(displ ,win ,gc, 5, 12, info_string, strlen(info_string));

    sprintf(info_string, "Running Time: %4.1f\0", TimeCounter);
    XDrawString(displ ,win ,gc, 5, 24, info_string, strlen(info_string));

    sprintf(info_string, "%4.1f fps\0", FPS);
    XDrawString(displ ,win ,gc, 5, 36, info_string, strlen(info_string));
}
const static void initGlobalGC(void) {
    gcvalues.foreground = 0xffffff;
    gcvalues.background = 0x000000;
    gcvalues.graphics_exposures = False;
    gc = XCreateGC(displ, win, GCBackground | GCForeground | GCGraphicsExposures, &gcvalues);
}
const static void pixmapcreate(void) {
    pixmap = XCreatePixmap(displ, win, wa.width, wa.height, wa.depth);
}
const static void pixmapdisplay(void) {
    XCopyArea(displ, pixmap, win, gc, 0, 0, wa.width, wa.height, 0, 0);
}
const static void atomsinit(void) {

    wmatom[Win_Close] = XInternAtom(displ, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(displ, win, &wmatom[Win_Close], 1);

    wmatom[Win_Name] = XInternAtom(displ, "WM_NAME", False);
    wmatom[Atom_Type] =  XInternAtom(displ, "STRING", False);
    XChangeProperty(displ, win, wmatom[Win_Name], wmatom[Atom_Type], 8, PropModeReplace, (unsigned char*)"Anvil", 5);
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
const static void initDependedVariables(void) {
    ASPECTRATIO = ((float)wa.width / (float)wa.height);
    HALFH = wa.height >> 1;
    HALFW = wa.width >> 1;

    /* Matrices initialization. */
    perspMat = perspectiveMatrix(FOV, ASPECTRATIO, ZNEAR, ZFAR);
}
/* Creates and Initializes the importand buffers. (frame, depth, shadow). */
const static void initBuffers(void) {
    frame_buffer = calloc(wa.width * wa.height * 4, 1);
}
/* General initialization and event handling. */
static int board(void) {

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
    atomsinit();
    registerSig(SIGSEGV);

    initDependedVariables();
    initBuffers();

    createScene(&scene);
    posWorldObjects(&scene);

    while (RUNNING) {

        clock_t start_time = start();
        UpdateTimeCounter();
        CalculateFPS();
        displayInfo();
        project();
        end(start_time);

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

    if (locale_init())
        fprintf(stderr, "Warning: main() -locale()\n");

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

