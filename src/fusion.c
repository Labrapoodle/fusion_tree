#include "fusion.h"

// O(1)
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
    if(amount <= 1) return (uint64_t)field;
    if (amount > BRANCHING_FACTOR) return ONES;
    uint64_t copyField = field;
    uint64_t tiledField = 0;
    uint64_t mask = ((1 << (amount << 8)) - 1);
    
    tiledField |= (copyField << 8);
    tiledField |= (tiledField << 16);
    tiledField |= (tiledField << 32); 

    return tiledField & mask;
}

// kinda sketch or pre-sketch
// O(BRANCHING_FACTOR)
uint8_t select(uint64_t X, uint64_t K)
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
uint64_t insertKey(uint64_t packedKeys, uint8_t key)
{
     //По-хорошему бы вставить проверки, но мы в AC0
    
    return insertField(packedKeys,key,packedRank(packedKeys,key));
}

//Заполняет все биты справа от pos значением бита на позиции pos
// pos 1-based
uint64_t fill(uint64_t key, uint8_t pos)
{
    if (pos > 64) return key; //Не знаю пока как избавиться от if, по условию если pos = 65, то вернуть key
    uint64_t bitToFill  = (key >> (pos-1)) & 1ULL;
    uint64_t wordOfBit = -bitToFill;      //Если наш бит 0, то всё слово 0, если 1, то 2^w-1, продолжаем бороться с ветвлением
    uint64_t mask = (2ULL << (pos - 1)) - 1;
    uint64_t filledmask = wordOfBit & mask;
    return (key & ~mask ) | filledmask;
}

fusNode* construct(int keys[BRANCHING_FACTOR], uint8_t amount)
{
    fusNode* node = (fusNode*)malloc(sizeof(*node));
    uint64_t packedImportantBits = 0;
    uint64_t packedKeys = 0;
    for(int i = 0; i < amount - 1; i++)
    {
        packedImportantBits = insertKey(packedImportantBits, msb((keys[i] ^ keys[i+1])));
    }
    for(int i = 0; i < amount ; i++)
    {
        packedKeys = insertKey(packedKeys, select(keys[i],packedImportantBits));
    }
    // Иницилизируем поля node
    node->packedImportantBits = packedImportantBits;
    node->packedKeys = packedKeys;
    return node;
}


uint8_t rank(uint64_t key, fusNode* node)
{
    uint8_t pos = 0;
    uint8_t i = packedRank(node->packedKeys,
                        select(key,node->packedImportantBits));         // D.1
    if(!i)
    {
        pos = msb(key ^ node->keys[0]);                                 // D.2
    }
    else
    {
        uint64_t previous = msb(key ^ node->keys[i]);       
        uint64_t next = msb(key ^ node->keys[i+1]);
        pos = (previous > next) ? previous : next;                      // D.3
    }

    uint64_t maskedSketch = fill(key,pos);                              // D.4
    uint8_t newPos = select(maskedSketch, node->packedImportantBits);   // D.5
    i = packedRank(node->packedKeys,newPos);                            // D.6
    uint8_t packedKeyI = (node->packedKeys >> ( i << 3) ) & 0x7f;       // 5'

    if(packedKeyI == newPos && key < node->keys[i])
    {
        if(i != 0)  i--;
    }
    
    return i;
}
