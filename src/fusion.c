#include "fusion.h"

// мб переделать от одной переменной
uint8_t msb(uint64_t u,uint64_t v)
{
    if(u==v) return 0;
    //(C1 - [(C1 - x AND C2) AND C1]) OR (x AND C1)
    printf("u^v: %lu\n", u^v);
    u = u ^ v;
    v = u;
    
    u &= C_2;
    u = C_1 - u;
    u &= C_1;
    u = C_1 - u;
    u |= v & C_1;
    uint8_t mainBites = (uint8_t)((u * (LEFT_SHIFT_BY_7)) >> (64-8));
    printf("mainBites: %u\n", mainBites);

    //Именно блок ненулевой
    uint8_t firstNonZero =  rank (TWOS_DEGREES, mainBites) -1;
    printf("firstNonZero %d; shift: %u\n",firstNonZero ,(uint8_t)((v >> (8*(firstNonZero))) ));
    uint8_t insideBlock = rank (TWOS_DEGREES, (uint8_t)(v >> (8*(firstNonZero))));


    return (firstNonZero*8) + insideBlock;

}

uint8_t rank(uint64_t packedKeys, uint8_t key)
{
    if(key>=128) return 8;
    printf("key: %u\n", key);
    // Делаем tiled key
    uint64_t x = ((uint64_t)key * K_MULTIPLIER) | K_ONES_MASK;
    printf("tiled K: %lu\n", x);
    //результат сравнения
    x = (x - packedKeys) & K_ONES_MASK;
    // собираем в меньшем байте
    x = (((x * K_STACKER) >> 55) & K_MASK) + ((x & 0x8000000000000000) >> 63);
    return (uint8_t) x;
}


fusNode* create_node(){
    fusNode* node = (fusNode*)malloc(sizeof(*node));
    node->keys[0] = ONES;
    node->keys[1] = ONES;
    node->keys[2] = ONES;

    
    node->C = 0;
    node->M = 0;
    node->D = 0;

    node->diverseBites[0]=65;
    node->diverseBites[1]=65;

    node->childs[0] = NULL;
    node->childs[1] = NULL;
    node->childs[2] = NULL;
    node->childs[3] = NULL;

    node->packedScethches = 0;

    return node;
}

void update_constants(fusNode *node)
{
    if(node->keys[0] == ONES) return;
    
    if(node->keys[1] != ONES) node->diverseBites[0] = msb(node->keys[0],node->keys[1]);
    if(node->keys[2] != ONES) node->diverseBites[1] = msb(node->keys[1],node->keys[2]);

    if(node->diverseBites[1] == node->diverseBites[0]) node->diverseBites[1] = 65;
    if(node->diverseBites[1] < node->diverseBites[0])
    {
        uint64_t temp = node->diverseBites[0];
        node->diverseBites[0] = node->diverseBites[1];
        node->diverseBites[1] = temp;
    }

    node->C = 0;
    if(node->diverseBites[0] != 65 ) node->C += 1 << (node->diverseBites[0]);
    if(node->diverseBites[0] != 65 ) node->C += 1 << (node->diverseBites[0]);

    node->M = 0;
    if(node->diverseBites[0] != 65 ) node->M = 1 << (MACHINE_WORD_SIZE - node->diverseBites[0]);
    if(node->diverseBites[1] != 65 )
    {
        uint8_t m2 = 
    }

}