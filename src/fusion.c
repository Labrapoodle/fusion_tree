#include "fusion.h"

// мб переделать от одной переменной
uint8_t msb(uint64_t u,uint64_t v)
{
    //(C1 - [(C1 - x AND C2) AND C1]) OR (x AND C1)
    u = u ^ v;
    v = u;
    
    u &= C_2;
    u = C_1 - u;
    u &= C_1;
    u = C_1 - u;
    u |= v & C_1;
    uint8_t mainBites = (uint8_t)(u * (LEFT_SHIFT_BY_7)) >> (64-8);
    uint8_t firstNonZero =  rank (TWOS_DEGREES, mainBites);

}

uint8_t rank(uint64_t packedKeys, uint8_t key)
{
    // Делаем tiled key
    key = (key * K_MULTIPLIER) | K_ONES_MASK;
    //результат сравнения
    key = (key - packedKeys) & K_ONES_MASK;
    // собираем в меньшем байте
    key = (((key * K_STACKER) >> 55) & K_MASK) + ((key & 0x8000000000000000) >> 63);
    return (uint8_t) key;
}