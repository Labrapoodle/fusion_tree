#define _POSIX_C_SOURCE 199309L
#include "fusion.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define AVERAGE_ITERATION 5000000 
#define MAX_KEYS_AMOUNT 3000000
#define EXPERIMENT_STEP 200000
#define BATCH_SIZE 10000 // Размер пачки случайных ключей

double wtime() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec * 1E-9;
}

int main() {
    fusNode* root = NULL;
    uint64_t *test_keys = malloc(sizeof(uint64_t) * BATCH_SIZE);

    printf("Starting experiments...\n");
    printf("%-10s | %-15s\n", "Keys", "Avg Time (ns)");
    printf("-------------------------------\n");

    for (uint64_t i = 0; i <= MAX_KEYS_AMOUNT; i++) {
        bTreeInsert(&root, i, 67);

        if (i > 0 && i % EXPERIMENT_STEP == 0) {
            // 1. Прогрев (наполняем кэш процессора данными дерева)
            for (int k = 0; k < 50000; k++) {
                lookup(root, getrand(0, i));
            }

            // 2. Подготовка случайных ключей для теста
            for (int k = 0; k < BATCH_SIZE; k++) {
                test_keys[k] = getrand(0, i);
            }

            // 3. Замер "шума" (сколько времени занимает сам цикл и индекс)
            double t_overhead_start = wtime();
            for (int j = 0; j < AVERAGE_ITERATION; j++) {
                uint64_t k = test_keys[j % BATCH_SIZE];
                (void)k; // Пустая операция
            }
            double overhead = wtime() - t_overhead_start;

            // 4. Реальный замер работы lookup
            // Используем volatile, чтобы компилятор не "выкинул" вызов функции
            volatile fusNode* res; 
            double t_start = wtime();
            for (int j = 0; j < AVERAGE_ITERATION; j++) {
                res = lookup(root, test_keys[j % BATCH_SIZE]);
            }
            double t_end = wtime();

            // 5. Вычисляем чистую разницу
            double total_pure_time = (t_end - t_start) - overhead;
            if (total_pure_time < 0) total_pure_time = 0; // На случай микро-погрешностей

            double avgTimeNs = (total_pure_time / AVERAGE_ITERATION) * 1E9;

            printf("%-10lu | %-15.3f ns\n", i, avgTimeNs);
        }
    }

    free(test_keys);
    freeBTree(&root);
    return 0;
}