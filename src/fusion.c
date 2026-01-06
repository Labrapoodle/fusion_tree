#include "fusion.h"

//Не забыть удалить
void printBinOfUINT(uint64_t n, char *buffer)
{
    for (int i = 63; i >= 0; i--) {
        // Shift number and check the LSB at each position
        buffer[63 - i] = (n & ((uint64_t)1 << i)) ? '1' : '0';
    }
    buffer[64] = '\0'; // Null-terminate the string
}

int getrand(int min, int max)
{
    return (double)rand() / (RAND_MAX + 1.0) * (max - min) + min;
}

// O(1)
// 0-based
int msb(uint64_t x)
{
    if (x == 0) return -1;
    int pos = 0;
    if (x & 0xFFFFFFFF00000000ULL) { x >>= 32; pos += 32; }
    if (x & 0xFFFF0000ULL) { x >>= 16; pos += 16; }
    if (x & 0xFF00ULL) { x >>= 8; pos += 8; }
    if (x & 0xF0ULL) { x >>= 4; pos += 4; }
    if (x & 0xCULL) { x >>= 2; pos += 2; }
    if (x & 0x2ULL) { x >>= 1; pos += 1; }
    return pos;

}

// O(1); 
uint64_t duplicate(uint8_t field, uint8_t amount)
{
    if(amount < 1 ) return 0ULL;
    if(amount == 1) return (uint64_t)field;
    if (amount > BRANCHING_FACTOR) return ONES;
    //uint64_t copyField = field;
    uint64_t tiledField = field;
    uint64_t mask = ((1 << (amount << 3)) - 1);
    if (amount == 8 ) mask = -1ULL;
    
    tiledField |= (tiledField << 8);
    tiledField |= (tiledField << 16);
    tiledField |= (tiledField << 32);
    
    

    return tiledField & mask;
}

// kinda sketch or pre-sketch
// O(BRANCHING_FACTOR)
uint8_t extractBits(uint64_t X, uint64_t K)
{
    uint64_t result = 0;
    int write_pos = 0; // Позиция, куда записываем бит в результат

    // Проходим по всем 8 полям (каждое по 8 бит)
    for (int i = 0; i < 8; i++) {
        // 1. Выделяем текущее поле (сдвигаем K вправо и берем младший байт)
        uint8_t field = (uint8_t)(K >> (i << 3));

        // 2. Проверяем сигнальный бит (самый левый в поле, т.е. 7-й бит)
        if (field & 0x80) { 
            // 3. Извлекаем индекс бита из оставшихся 7 бит (0x7F = 01111111)
            int bit_to_get = field & 0x7F;

            // 4. Достаем значение этого бита из X
            // Сдвигаем X вправо на bit_to_get и берем последний бит
            uint64_t bit_value = (X >> bit_to_get) & 1;

            // 5. Помещаем этот бит в результат на текущую свободную позицию
            result |= (bit_value << write_pos);

            // 6. Сдвигаем позицию записи для следующего найденного поля
            write_pos++;
        }
    }
    return result;
}

// По-простому просто кол-во хранимых ключей 
// Но вообще наибольший индекс непустого поля 
//  в packedKeys при отсчёте справа + 1
// O(1)
uint8_t leftField(uint64_t packedKeys)
{
    return (uint8_t)(msb(packedKeys) >> LOG_LOG_W) + 1; //По-хорошему бы вставить проверки, что msb(x) != -1, но мы в AC0
}

// Возвращает маску соответствующую единицам
//  в тестовых битах в первых amount количестве полей
// O(1)
uint64_t fillTestBits(uint8_t amount)
{
    return duplicate((uint8_t)0x80,amount); 
}

// Pre-rank
// 1-based
// O(1)
uint8_t packedRank(uint64_t packedKeys, uint8_t key)
{
    uint8_t keyAmount = leftField(~packedKeys);      
    uint64_t mask = fillTestBits(keyAmount);
    uint64_t tiledX = duplicate(key,keyAmount) | mask;
    uint64_t result = (tiledX - packedKeys) & mask;
    return leftField(result);
}

// Возвращает новый packedKeys с полем field
//  вставленным в поле номером position
// O(1)
uint64_t insertField(uint64_t packedKeys, uint8_t field, uint8_t position)
{
    
    
    uint64_t copyField = field & 0x7FULL;   // На всякий случай 
        
    //Если packedKeys содержит ключи [K3​,K2​,K1​,K0​] и мы вставляем X в position = 2
    uint64_t mask1 = (1ULL <<  (position << 3) ) - 1;       //mask1 выделяет [K1​,K0​].
    uint64_t mask2 = ~mask1;                                //mask2 выделяет [K3​,K2​,0,0].

    
    uint8_t shift  = (position)  << 3;                      //(packedKeys & mask2) << 8 превращает это в [K3​,K2​,0,0,0]
    
    //copyField << shift создает [0,0,X,0,0]
    //packedKeys & mask1 дает [0,0,0,K1​,K0​].
    return (((packedKeys & mask2) << 8) | (copyField << shift)) | (packedKeys & mask1);
}

// Возвращает новый packedKeys с вставленным ключом key
// O(1)
// Костыль в виде -1, т.к. packedRank 1-based
uint64_t insertKey(uint64_t packedKeys, uint8_t key)
{
     //По-хорошему бы вставить проверки
   
    return insertField(packedKeys,key,packedRank(packedKeys,key));
}

//Заполняет все биты справа от pos значением бита на позиции pos
// pos 0-based
uint64_t fill(uint64_t key, uint8_t pos)
{
    if (pos < 1 || pos > 63) return key; // По условию если pos = 65, то вернуть key
    uint64_t bitToFill  = (key >> (pos)) & 1ULL;
    
    uint64_t wordOfBit = -bitToFill;      //Если наш бит 0, то всё слово 0, если 1, то 2^w-1
    
    uint64_t mask = (1ULL << (pos )) - 1;
    uint64_t filledmask = wordOfBit & mask;
    return (key & ~mask ) | filledmask;
}

void construct(fusNode *node)
{
    
    uint64_t packedImportantBits = 0;
    uint64_t packedKeys = ONES;

    for(int i = 0; i < node->amount - 1; i++)
    {
        packedImportantBits = insertKey(packedImportantBits, msb((node->keys[i] ^ node->keys[i+1])));
    }
    uint64_t maskForBites = fillTestBits(node->amount - 1);
    packedImportantBits |= maskForBites;
    for(int i = 0; i < node->amount ; i++)
    {
        packedKeys = insertKey(packedKeys, extractBits(node->keys[i],packedImportantBits));               
    }
    
    
    
    // Иницилизируем поля node
    node->packedImportantBits = packedImportantBits;
    node->packedKeys = packedKeys;
    
    
}


uint8_t rank(uint64_t key, fusNode* node)
{
    uint8_t pos = 0;
    uint8_t i = packedRank(node->packedKeys,  extractBits(key,node->packedImportantBits)) ;         // D.1                          

    if(!i)
    {
        pos = msb(key ^ node->keys[0]);                                     // D.2
    }
    else if(i >= node->amount)
    {
        pos = msb(key ^ node->keys[node->amount - 1]);
    }
    else
    {
        uint64_t previous = msb(key ^ node->keys[i-1]);       
        uint64_t next = msb(key ^ node->keys[i]);
        if(previous == ONES) previous = 0;
        if(next == ONES) next = 0;
        pos = (previous < next) ? previous : next;                          // D.3
    }

    uint64_t maskedSketch = fill(key,pos);                                      // D.4
    

    uint8_t newPos = extractBits(maskedSketch, node->packedImportantBits);  // D.5
    i = packedRank(node->packedKeys,newPos);                                    // D.6

    if(i == 0) return 0;
    uint8_t preHofKeys = (node->packedKeys >> ((i-1) << 3)) & 0x7F;
    if( preHofKeys == newPos && key < node->keys[i-1]) i--;
    return i;

   //if (i > 0) {
   //     // Проверяем ключ непосредственно перед i
   //     if (key <= node->keys[i - 1]) {
   //         return i - 1;
   //     }
   // }
//
   // // Если i == 0 или ключ всё же больше текущего (i-1), 
   // // возвращаем i, но не больше общего количества ключей.
   // return (i > node->amount) ? node->amount : i;
}
