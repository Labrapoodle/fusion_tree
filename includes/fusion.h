#ifndef FUSION_H
#define FUSION_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// в packedKeys неиспользованные поля заполнены единицами (даже test bits)


// С test bit 8
#define FIELD_SIZE 8
//Пригодится ровно в одном месте
#define LOG_LOG_W 3
// Максимальное значение, чтобы скетчи ключей влезали в машинное слово
#define BRANCHING_FACTOR 8
#define MACHINE_WORD_SIZE 64
#define ONES 0xFFFFFFFFFFFFFFFF
//для rank
#define K_MULTIPLIER 0x0101010101010101
#define K_ONES_MASK 0x8080808080808080
#define K_STACKER 0b1000000010000000100000001000000010000000100000001
#define K_MASK 0b1111


// Для msb
#define MSB_BLOCKS_AMOUNT 8
#define C_1 0x8080808080808080
#define C_2 0x7F7F7F7F7F7F7F7F
#define LEFT_SHIFT_BY_7 0b10000001000000100000010000001000000100000010000001
#define TWOS_DEGREES 0x8040201008040201

#define MAX_KEY_DIFFERS 2

typedef struct fusNode fusNode;

struct fusNode
{
    uint64_t keys[BRANCHING_FACTOR];     
    fusNode* childs[BRANCHING_FACTOR +1];

    uint8_t amount;

    uint64_t packedImportantBits;
    uint64_t packedKeys;
};

int msb(uint64_t x);
uint64_t duplicate(uint8_t field, uint8_t amount);
uint8_t extractBits(uint64_t X, uint64_t K);
uint8_t leftField(uint64_t packedKeys);
uint64_t fillTestBits(uint8_t amount);
uint8_t packedRank(uint64_t packedKeys, uint8_t key);
uint64_t insertField(uint64_t packedKeys, uint8_t field, uint8_t position);
uint64_t insertKey(uint64_t packedKeys, uint8_t key);
uint64_t fill(uint64_t key, uint8_t pos);
void construct(fusNode *node);

uint8_t rank(uint64_t key, fusNode* node);


void printBinOfUINT(uint64_t n, char *buffer);
int getrand(int min, int max);

#endif