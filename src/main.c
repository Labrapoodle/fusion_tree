
#define _POSIX_C_SOURCE 199309L
#include "fusion.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define AVERAGE_ITERATION 5000000 
#define MAX_KEYS_AMOUNT 40
#define EXPERIMENT_STEP 200000
#define BATCH_SIZE 10000 // Размер пачки случайных ключей

double wtime() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec * 1E-9;
}

int main() {
    fusNode* root = NULL;
    

        

    for (uint64_t i = 0; i <= MAX_KEYS_AMOUNT; i++) {
        bTreeInsert(&root, i, 67);

         
    }

    printTree(root, 0,0);

    bTreeDelete(&root, 34);

    printf("\033[32m====================\033[0m\n");

    printTree(root,0,0);

    bTreeDelete(&root, 33);
    printf("\033[33m====================\033[0m\n");
    printTree(root,0,0);

    bTreeDelete(&root, 32);
    printf("\033[33m====================\033[0m\n");
    printTree(root,0,0);

    bTreeDelete(&root, 31);
    printf("\033[33m====================\033[0m\n");
    printTree(root,0,0);

    bTreeDelete(&root, 25);
    bTreeDelete(&root, 26);
    bTreeDelete(&root, 27);
    bTreeDelete(&root, 28);
    printf("\033[33m====================\033[0m\n");
    printTree(root,0,0);


    freeBTree(&root);
    return 0;
}
