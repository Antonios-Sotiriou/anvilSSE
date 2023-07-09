#ifndef _GENERAL_FUNCTIONS_H
#define _GENERAL_FUNCTIONS_H 1

#ifndef _STDIO_H
    #include <stdio.h>
#endif

#ifndef _STDLIB_H
    #include <stdlib.h>
#endif

#ifndef _STRING_H
    #include <string.h>
#endif

void *create2darray(void **obj, const unsigned long obj_size, const int height, const int width);
void *resize2darray(void **obj, const unsigned long obj_size, const int height, const int width);
const void free2darray(void **obj, const int height);
const void swap(void *a, void *b, unsigned long size);
const float radians(const float value);

#endif /* _GENERAL_FUNCTIONS_H */


