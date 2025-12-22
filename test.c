#include <stdint.h>
#include <stdio.h>

int main(){

    uint64_t degrees = 0x8040201008040201;
    //degrees = 1;
    //degrees += 2<<8;
    //degrees += 4<<16;
    //degrees += 8<<24;
    //degrees += 16<<32;
    //degrees += 32<<40;
    //degrees += 64<<48;
    //degrees += 128<<56;
    printf("degrees: %lu\n", degrees);
    return 0;
}