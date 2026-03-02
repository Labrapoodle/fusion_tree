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
    uint64_t mask = ((1ULL << ((uint64_t)amount << 3)) - 1);

    

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
    return (uint8_t)(msb(packedKeys) >> LOG_LOG_W) + 1; 
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
    
    //uint64_t packedImportantBits = 0;
    uint64_t packedKeys = ONES;

    if(node->amount != 0)
    {
        // 1. Собрали позиции всех различающихся битов (их максимум amount-1)
        uint8_t bits[8]; 
        int n = 0;
        for (int i = 0; i < node->amount - 1; i++) {
            bits[n++] = msb(node->keys[i] ^ node->keys[i+1]);
        }

        // 2. Сортируем позиции (любым способом)
        for (int i = 0; i < n-1; i++) {
            for (int j = 0; j < n-i-1; j++) {
                if (bits[j] > bits[j+1]) {
                    uint8_t tmp = bits[j]; bits[j] = bits[j+1]; bits[j+1] = tmp;
                }
            }
        }

        // 3. Упаковываем только уникальные значения
        node->packedImportantBits = 0;
        int unique_idx = 0;
        for (int i = 0; i < n; i++) {
            if (i > 0 && bits[i] == bits[i-1]) continue;

            // Записываем позицию бита в очередной байт
            node->packedImportantBits |= ((uint64_t)bits[i] << (unique_idx << 3));
            unique_idx++;
        }
        node->packedImportantBits |= fillTestBits((uint8_t)unique_idx);
    }
    
    
    for(int i = 0; i < node->amount ; i++)
    {
        packedKeys = insertKey(packedKeys, extractBits(node->keys[i],node->packedImportantBits));               
    }
    
    
    
    // Иницилизируем поля node
    //node->packedImportantBits = packedImportantBits;
    node->packedKeys = packedKeys;
    
    
}


uint8_t rank(uint64_t key, fusNode* node)
{
    uint8_t pos = 0;
    uint8_t i = packedRank(node->packedKeys,  extractBits(key,node->packedImportantBits)) ;         // D.1
    

    if(!i) pos = msb(key ^ node->keys[0]);                                                          // D.2    
    else if(i >= node->amount) pos = msb(key ^ node->keys[node->amount - 1]);    
    else
    {
        uint64_t previous = msb(key ^ node->keys[i-1]);       
        uint64_t next = msb(key ^ node->keys[i]);
        if(previous == ONES) previous = 0;
        if(next == ONES) next = 0;
        pos = (previous < next) ? previous : next;                                                  // D.3
    }

    uint64_t maskedSketch = fill(key,pos);                                                          // D.4
    

    uint8_t newPos = extractBits(maskedSketch, node->packedImportantBits);                          // D.5
    i = packedRank(node->packedKeys,newPos);                                                        // D.6

    if(i == 0) return 0;
    uint8_t preHofKeys = (node->packedKeys >> ((i-1) << 3)) & 0x7F;
    if( preHofKeys == newPos && key < node->keys[i-1]) i--;
    return i;

   
}


// NOW B-TREES

fusNode* create_node()
{
    fusNode* node =(fusNode*)malloc(sizeof(*node));
    for(int i = 0; i < BRANCHING_FACTOR; i++)
    {
        node->keys[i] = 0;
        node->values[i] = -1;
        node->childs[i] = NULL;
    }
    node->childs[BRANCHING_FACTOR] = NULL;
    node->amount = 0;
    construct(node);
    return node;
}

fusNode* lookup(fusNode* root, uint64_t key)
{   
    
    fusNode* node = root;
    while(node != NULL)
    {
        
        uint8_t rk = rank(key, node);
        
        

        


        if(rk != 0 && key == node->keys[rk-1]) return node;     
            
         
        
        node = node->childs[rk];
        
        
    }
    
    // Тут вернёт NULL
    return node;
}

void splitChild(fusNode* parent, uint8_t index, fusNode* child) {
    fusNode* newNode = create_node(); 
    newNode->isLeaf = child->isLeaf;
    
    // Если BRANCHING_FACTOR 8, то:
    // child: [0, 1, 2, 3] | 4 | [5, 6, 7]
    // newNode получает 3 ключа: 5, 6, 7
    newNode->amount = 3;
    for (int j = 0; j < 3; j++) {
        newNode->keys[j] = child->keys[j + 5];
        newNode->values[j] = child->values[j + 5];
    }

    if (!child->isLeaf) {
        // Детей всегда на 1 больше: 5, 6, 7, 8
        for (int j = 0; j < 4; j++) {
            newNode->childs[j] = child->childs[j + 5];
        }
    }
    child->amount = 4; // В старом осталось 0, 1, 2, 3

    // Сдвигаем детей в родителе под новый newNode
    for (int j = parent->amount; j >= index + 1; j--) {
        parent->childs[j + 1] = parent->childs[j];
    }
    parent->childs[index + 1] = newNode;

    // Сдвигаем ключи в родителе под медиану (индекс 4)
    for (int j = parent->amount - 1; j >= (int)index; j--) {
        parent->keys[j + 1] = parent->keys[j];
        parent->values[j + 1] = parent->values[j];
    }
    
    parent->keys[index] = child->keys[4];
    parent->values[index] = child->values[4];
    parent->amount++;

    // ОЧЕНЬ ВАЖНО: обнуляем старые места в child, чтобы они не мешали
    for(int j = 4; j < 8; j++) {
        child->keys[j] = 0;
        child->values[j] = -1;
    }

    construct(child);
    construct(newNode);
    construct(parent);
}

void insertNonFull(fusNode* node, uint64_t key, int value) {
    uint8_t i = rank(key, node);

    if (node->isLeaf) {
        // Мы в листе, сдвигаем элементы и вставляем
        for (int j = node->amount - 1; j >= (int)i; j--) {
            node->keys[j + 1] = node->keys[j];
            node->values[j + 1] = node->values[j];
        }
        node->keys[i] = key;
        node->values[i] = value;
        node->amount++;
        
        // Обновляем битовую магию
        construct(node);
    } else {
        // Мы во внутреннем узле, спускаемся к ребенку
        // Но сначала проверяем, не полон ли он
        if (node->childs[i]->amount == 8) {
            splitChild(node, i, node->childs[i]);
            // После разделения медиана ушла наверх, и у нас теперь два ребенка.
            // Нужно решить, в какой из них идти.
            if (key > node->keys[i]) {
                i++;
            }
        }
        insertNonFull(node->childs[i], key, value);
    }
}

void bTreeInsert(fusNode** rootAddr, uint64_t key, int value) {
    if (*rootAddr == NULL) {
        // Дерево пустое, создаем первый корень
        fusNode* node = (fusNode*)malloc(sizeof(fusNode));
        node->amount = 1;
        node->keys[0] = key;
        node->values[0] = value;
        node->isLeaf = 1;
        construct(node);
        *rootAddr = node;
        return;
    }

    fusNode* root = *rootAddr;

    // Если корень полон (8 ключей), дерево должно вырасти вверх
    if (root->amount == 8) {
        fusNode* newRoot = (fusNode*)malloc(sizeof(fusNode));
        newRoot->isLeaf = 0;
        newRoot->amount = 0;
        newRoot->childs[0] = root;

        // Разделяем старый корень, медиана пойдет в newRoot
        splitChild(newRoot, 0, root);

        // Обновляем внешний указатель СРАЗУ
        *rootAddr = newRoot;

        // Вставляем значение в один из новых путей
        insertNonFull(newRoot, key, value);
    } else {
        insertNonFull(root, key, value);
    }
}


void borrowFromPrev(fusNode* parent, int idx) {
    fusNode* child = parent->childs[idx];
    fusNode* sibling = parent->childs[idx - 1];

    // Сдвигаем всё в child вправо, чтобы освободить место в начале
    for (int i = child->amount - 1; i >= 0; i--) {
        child->keys[i + 1] = child->keys[i];
        child->values[i + 1] = child->values[i];
    }
    if (!child->isLeaf) {
        for (int i = child->amount; i >= 0; i--)
            child->childs[i + 1] = child->childs[i];
    }

    // Ключ из родителя опускается в ребенка
    child->keys[0] = parent->keys[idx - 1];
    child->values[0] = parent->values[idx - 1];
    if (!child->isLeaf) child->childs[0] = sibling->childs[sibling->amount];

    // Ключ из соседа поднимается в родителя
    parent->keys[idx - 1] = sibling->keys[sibling->amount - 1];
    parent->values[idx - 1] = sibling->values[sibling->amount - 1];

    child->amount++;
    sibling->amount--;

    construct(child); construct(sibling); construct(parent);
}
// Аналогично пишется borrowFromNext...

void borrowFromNext(fusNode* parent, int idx) {
    fusNode* child = parent->childs[idx];
    fusNode* sibling = parent->childs[idx + 1];

    

    // Ключ из родителя опускается в ребенка
    child->keys[child->amount ] = parent->keys[idx ];
    child->values[child->amount ] = parent->values[idx ];
    if (!child->isLeaf) child->childs[child->amount + 1] = sibling->childs[0];

    child->amount++;

    // Ключ из соседа поднимается в родителя
    parent->keys[idx] = sibling->keys[0];
    parent->values[idx ] = sibling->values[0];

    for (int i = 1; i < sibling->amount; i++) {
        sibling->keys[i - 1] = sibling->keys[i];
        sibling->values[i - 1] = sibling->values[i];
    }

    if (!sibling->isLeaf) {
        for (int i = 1; i <= sibling->amount; i++) {
            sibling->childs[i - 1] = sibling->childs[i];
        }
    }


    
    sibling->amount--;

    construct(child); construct(sibling); construct(parent);
}

void mergeNodes(fusNode* parent, int idx) {
    fusNode* left = parent->childs[idx];
    fusNode* right = parent->childs[idx + 1];

    // Вытягиваем ключ из родителя в левый узел
    left->keys[left->amount] = parent->keys[idx];
    left->values[left->amount] = parent->values[idx];

    // Копируем всё из правого в левый
    for (int i = 0; i < right->amount; i++) {
        left->keys[left->amount + 1 + i] = right->keys[i];
        left->values[left->amount + 1 + i] = right->values[i];
    }
    if (!left->isLeaf) {
        for (int i = 0; i <= right->amount; i++)
            left->childs[left->amount + 1 + i] = right->childs[i];
    }

    left->amount += right->amount + 1;

    // Сдвигаем ключи в родителе
    for (int i = idx + 1; i < parent->amount; i++) {
        parent->keys[i - 1] = parent->keys[i];
        parent->values[i - 1] = parent->values[i];
        parent->childs[i] = parent->childs[i + 1];
    }
    parent->amount--;

    construct(left); construct(parent);
    free(right);
}

void deleteFromNode(fusNode* node, uint64_t key) {
    uint8_t i = rank(key, node);

    // СЛУЧАЙ 1: Ключ найден в текущем узле
    if (i > 0 && node->keys[i - 1] == key) {
        if (node->isLeaf) {
            // Просто удаляем из листа
            for (int j = i; j < node->amount; j++) {
                node->keys[j - 1] = node->keys[j];
                node->values[j - 1] = node->values[j];
            }
            node->amount--;
            construct(node);
        } else {
            // Ключ во внутреннем узле - заменяем на предшественника
            fusNode* predNode = node->childs[i - 1];
            while (!predNode->isLeaf) predNode = predNode->childs[predNode->amount];
            uint64_t predKey = predNode->keys[predNode->amount - 1];
            int predVal = predNode->values[predNode->amount - 1];
            
            node->keys[i - 1] = predKey;
            node->values[i - 1] = predVal;
            deleteFromNode(node->childs[i - 1], predKey);
            construct(node);
        }
    } 
    // СЛУЧАЙ 2: Ключ не в этом узле
    else if (!node->isLeaf) {
        // Проверка: достаточно ли ключей в ребенке для спуска?
        if (node->childs[i]->amount < 4) {
            // Нужно усилить ребенка через Borrow или Merge
            if (i > 0 && node->childs[i - 1]->amount >= 4) borrowFromPrev(node, i);
            else if (i < node->amount && node->childs[i + 1]->amount >= 4) borrowFromNext(node, i);
            else {
                if (i < node->amount) mergeNodes(node, i);
                else mergeNodes(node, i - 1);
                // После слияния индекс i мог измениться, нужно обновить его
                i = rank(key, node); 
            }
        }
        deleteFromNode(node->childs[i], key);
    }
}

void bTreeDelete(fusNode** rootAddr, uint64_t key) {
    if (*rootAddr == NULL) return;

    fusNode* root = *rootAddr;
    
    // Вызываем основную логику удаления (ту, что мы писали ранее)
    deleteFromNode(root, key);

    // Если корень стал пустым после удаления (у него 0 ключей)
    // Это значит, что корень не был листом и его дети слились
    if (root->amount == 0) {
        fusNode* oldRoot = root;
        if (!root->isLeaf) {
            // Ребенок становится новым корнем
            *rootAddr = root->childs[0];
        } else {
            // Дерево стало полностью пустым
            *rootAddr = NULL;
        }
        free(oldRoot);
    }
}

void freeBTree(fusNode** rootAddr) {
    if (*rootAddr == NULL) return;

    fusNode* node = *rootAddr;

    // 1. Если узел не является листом, сначала рекурсивно удаляем всех детей
    if (!node->isLeaf) {
        for (int i = 0; i <= node->amount; i++) {
            if (node->childs[i] != NULL) {
                // Рекурсивный вызов для каждого ребенка
                freeBTree(&(node->childs[i]));
            }
        }
    }

    // 2. После того как все дети удалены (или если это был лист), 
    // удаляем сам узел
    free(node);

    // 3. Зануляем указатель, чтобы внешний код знал, что дерева больше нет
    *rootAddr = NULL;
}

void printTree(fusNode* node, int level, int childIdx) {
    if (node == NULL) return;

    // Печатаем отступ
    for (int i = 0; i < level; i++) printf("    ");

    // Выводим индекс ребенка, чтобы видеть связь с родителем
    if (level > 0) printf("Child[%d] -> ", childIdx);
    else printf("ROOT -> ");

    // Выводим ключи
    printf("[ ");
    for (int i = 0; i < node->amount; i++) {
        printf("%llu", (unsigned long long)node->keys[i]);
        if (i < node->amount - 1) printf(" | ");
    }
    printf(" ] (%s, amount: %d)\n", node->isLeaf ? "Leaf" : "Internal", node->amount);

    // Рекурсия для детей
    if (!node->isLeaf) {
        for (int i = 0; i <= node->amount; i++) {
            printTree(node->childs[i], level + 1, i);
        }
    }

    
}

int getHeight(fusNode* node) {
    if (node == NULL) {
        return 0;
    }
    
    // Если у узла нет первого ребенка, значит это лист
    // (учитывая твою структуру, где childs[0] указывает на поддерево)
    if (node->childs[0] == NULL) {
        return 1;
    }
    
    return 1 + getHeight(node->childs[0]);
}