#ifndef FUSION_H
#define FUSION_H
#include <stdint.h>
#include <math.h>


// Максимальное значение, чтобы скетчи ключей влезали в машинное слово
#define BRANCHING_FACTOR 3
#define MACHINE_WORD_SIZE 64
// Для msb
#define MSB_BLOCKS_AMOUNT 8
#define C_1 0x8080808080808080
#define C_2 0x7F7F7F7F7F7F7F7F
#define LEFT_SHIFT_BY_7 0x0807060504030201
//10000001000000100000010000001000000100000010000001
typedef struct fusNode fusNode;

struct fusNode
{
    uint64_t keys[BRANCHING_FACTOR];
    uint8_t *diverseBites;
    uint64_t packedScethches;

    // Константы для вычисления ~скетча, название из ориг. статьи
    uint64_t C;
    uint64_t M;
    uint64_t D;


};

uint8_t msb(uint64_t u, uint64_t v);




#endif