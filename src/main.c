#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define ITERATIONS 1000000

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

uint64_t result(uint8_t keys[], uint8_t compare){
    uint64_t  rank = 0;
    
    for(int i = 0; i < 8; i++){
        
        if(keys[i] <= compare) rank++;
    }
    
    return rank;
}

int main(){
    srand(time(NULL));
    const uint64_t kMultiplier = 0b100000001000000010000000100000001000000010000000100000001;
    const uint64_t kOnesMask = 0b1000000010000000100000001000000010000000100000001000000010000000;
    const uint64_t kStacker = 0b1000000010000000100000001000000010000000100000001;
    const uint64_t mainBite = 0x8000000000000000;
    const uint8_t kShift = 55;
    const uint64_t kMask = 0b1111;

    double parallelTime = 0;
    


    uint8_t keys[8];
    


    double t = wtime();
    uint64_t packedKeys = 0;
    for(int i = 0; i < ITERATIONS; i++)
    {
        uint8_t k = getrand(0,127);
        for(int i = 0; i < 8 ; i++){
            keys[i] = getrand(0,127);
        }
        memcpy(&packedKeys, keys, 8);
        //printf("\033[34m \033[0m\n");  
        uint64_t tiledK = (k * kMultiplier) | kOnesMask;    
        uint64_t comparison = (tiledK - packedKeys) & kOnesMask; 
        uint64_t rank = (((comparison * kStacker) >> kShift) & kMask) + ((comparison & mainBite) >> 63);

        //uint64_t rank = (uint64_t)kekrank;
        parallelTime = wtime()-t;
        if((rank-result(keys, k))!=0) printf("nan\n");
        printf("bit: %lu, compare: %lu\n", rank,result(keys, k));
        
    }
    
    //55
    return 0;
}