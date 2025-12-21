#include "fusion.h"

// мб переделать от одной переменной
uint8_t msb(uint64_t u,uint64_t v){
    //(C1 - [(C1 - x AND C2) AND C1]) OR (x AND C1)
    u = u ^ v;
    v = u;
    
    u &= C_2;
    u = C_1 - u;
    u &= C_1;
    u = C_1 - u;
    u |= v & C_1;

    

}
