#include "../headers/exec_time.h"

const clock_t start(void) {
    return clock();
}
const float end(const clock_t start_time) {
    clock_t end_time = clock();
    double exec_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    fprintf(stdout, "Execution Time : %f\n", exec_time);
    return (float)exec_time;
}

