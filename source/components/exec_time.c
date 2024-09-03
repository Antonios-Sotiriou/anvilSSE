#include "../../headers/components/exec_time.h"

const clock_t start(void) {
    return clock();
}
const float end(const clock_t start_time) {
    clock_t end_time = clock();
    double exec_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    return (float)exec_time;
}

