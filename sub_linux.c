// (const char *)ptr - pass your device like "/dev/input/event2" here
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/input.h>

#ifndef GET_COORD
#define GET_COORD

int getCoordinates() {
    // in this file events from joystick are being sent
    int fd = open("/dev/input/mice", O_RDONLY);
    struct input_event ev;
    unsigned char *ptr = (unsigned char*)&ev;
    unsigned char b1;
    char value = 'C';
    char value_new;
    // C -> center

    while (read(fd, &ev, sizeof(struct input_event))) {
        b1 = ptr[0]&0x1;
        if (b1 == 1) {
            printf("%s","Switching to manual controls ....\n");
            if (((int)ptr[1] >= 200) ) {
                printf("%s","Left arc\n");
            }
            else if (((int)ptr[1] == 0) && ((int)ptr[2] < 100) && ((int)ptr[2] > 5)) {  
                printf("%s","center\n");
            } else if ((int)ptr[1] < 45 && ((int)ptr[1] > 1)){
                printf("%s","Right arc\n");
            }
        }
    }
    close(fd);
    return 0;
}

#endif
