#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

// 0...127
int getrand(int min, int max)
{
    return (double)rand() / (RAND_MAX + 1.0) * (max - min) + min;
}

void result(uint8_t keys[], uint8_t compare){
    uint8_t  rank = 0;
    double t = wtime();
    for(int i = 0; i < 4; i++){
        
        if(keys[i] <= compare) rank++;
    }
    t = wtime()-t;
    printf("\n\033[33m Cycle-rank: %d, time: %.60f\033[0m\n", rank, t);
}

int main(){
    srand(time(NULL));
    const uint64_t kMultiplier = 0b100000001000000010000000100000001000000010000000100000001;
    const uint64_t kOnesMask = 0b1000000010000000100000001000000010000000100000001000000010000000;
    const uint64_t kStacker = 0b100000001000000010000000100000001000000010000000100000001;
    const uint8_t kShift = 31;
    const uint64_t kMask = 0b111;

    double parallelTime = 0;
    


    uint8_t keys[4];
    uint8_t k = getrand(0,127);
    for(int i = 0; i < 4 ; i++){
        keys[i] = getrand(0,127);
    }

    double t = wtime();
    uint64_t packedKeys = 0;
    memcpy(&packedKeys, keys, 4);
    //printf("\033[34m \033[0m\n");  
    uint64_t tiledK = (k * kMultiplier) | kOnesMask;    
    uint64_t comparison = (tiledK - packedKeys) & kOnesMask; 
    uint64_t rank = ((comparison * kStacker) >> kShift) & kMask;
    parallelTime = wtime()-t;

    printf("\033[32mrank: %lu, time: %.60f\033[0m\n",rank,parallelTime);
    result(keys, k);

    return 0;
}