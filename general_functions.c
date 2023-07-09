#include "headers/general_functions.h"

/* Creates an obj 2d array of height and width with obj_size of each entry. */
void *create2darray(void **obj, const unsigned long obj_size, const int height, const int width) {
    obj = malloc(sizeof(obj) * height);
    if (obj == NULL)
        fprintf(stderr, "Could not allocate 2D array height memory! create2darray() -- malloc().\n");

    for (int y = 0; y < height; y++) {
        obj[y] = malloc(obj_size * width);
        if (obj[y] == NULL)
            fprintf(stderr, "Could not allocate 2D array width memory! create2darray() -- malloc().\n");
    }
    return obj;
}
/* Resizes an obj 2d array of height and width with obj_size of each entry. */
void *resize2darray(void **obj, const unsigned long obj_size, const int height, const int width) {
    obj = realloc(obj, sizeof(obj) * height);
    if (obj == NULL)
        fprintf(stderr, "Could not reallocate texels 2D array memory! resize2darray() -- malloc().\n");

    for (int y = 0; y < height; y++) {
        obj[y] = realloc(obj[y], obj_size * width);
        if (obj[y] == NULL)
            fprintf(stderr, "Could not reallocate 2D array width memory! resize2darray() -- malloc().\n");
    }
    return obj;
}
/* Frees all resources of an obj 2d array of height. */
const void free2darray(void **obj, const int height) {
    for (int y = 0; y < height; y++)
        free(obj[y]);
    free(obj);
}
/* Swaping two variables a and b of any type with size. */
const void swap(void *a, void *b, unsigned long size) {
    void *temp = malloc(size);
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
    free(temp);
}
const float radians(const float value) {
    return value * (3.14159 / 180.0);
}

