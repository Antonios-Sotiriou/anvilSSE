#include "click_select.h"


const int clickSelect(XEvent *ev) {
    
    const float click_depth = (ev->xbutton.y * main_wa.width) + ev->xbutton.x;

    printf("depth on screen space: %f\n", main_depth_buffer[click]);
}

