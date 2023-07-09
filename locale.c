// general headers
#ifndef _STDIO
    #include <stdio.h>
#endif
#ifndef _STDLIB_H
    #include <stdlib.h>
#endif
#ifndef _X11_XLIB_H_
    #include <X11/Xlib.h>
#endif
#ifndef _X11_XLOCALE_H_
    #include <X11/Xlocale.h>
#endif

// Locale optimisation.Returns 0 if no error 1 otherwise.
const int locale_init(void) {

    if (setlocale(LC_ALL, "") == NULL) {
        perror("setlocale()");
        return EXIT_FAILURE;
    }
    if (!XSupportsLocale()) {
        perror("XSupportsLocale()");
        return EXIT_FAILURE;
    }
    if (XSetLocaleModifiers("") == NULL) {
        perror("XSetLocaleModifiers()");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

