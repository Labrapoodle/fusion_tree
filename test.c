#include "fusion.h"

int main(){

    uint64_t zero = 0;
    uint64_t one = 1ULL;

    printf("zero: %lu, ~zero: %lu, \n one: %lu, ~one: %lu\n",
        zero, -zero, one, -one);

    return 0;
}