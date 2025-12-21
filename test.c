#include <stdint.h>
#include <stdio.h>

int main(){

    uint64_t mine = 0b10000001000000100000010000001000000100000010000001;
    uint64_t AIs = 0x8040201008040201;
    
    uint64_t test = 0b1000000000000000000000001000000010000000100000000000000010000000;
    uint8_t etalone = 0b10011101;

    printf("etalone: %d\n", etalone);
    uint64_t result = (test * mine) >> (64-8);
    printf("mine: result-etalone is %ld, result: %ld\n", result-etalone, result);
    result = (test * AIs) >> (64-8);
    printf("AIs: result-etalone is %ld, result: %ld\n", result-etalone, result);

    return 0;
}