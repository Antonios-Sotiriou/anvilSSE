#ifndef _EXEC_TIME_H
#define _EXEC_TIME_H 1

#ifndef _STDIO_H
    #include <stdio.h>
#endif

#ifndef _TIME_H
    #include <time.h>
#endif

const clock_t start(void);
const float end(const clock_t start_time);

#endif /* _EXEC_TIME_H */

