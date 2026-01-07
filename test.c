#include "fusion.h"

#define ASSERT_TEST(cond, msg) if(!(cond)) { printf("\033[31mFAIL: %s\033[0m\n", msg); }

void success_print(){
    printf("\033[32m good \033[0m\n");
}

void error_print(int id ){
    printf("\033[31m not good \033[0m, id: %d\n", id);
}




void msb_testing()
{
    uint64_t one = 1ULL;
    uint64_t sec = 0x80ULL;
    uint64_t max1 = 0x8000000000000000;
    uint64_t max2 = ONES;

    if(msb(one) != 0) error_print(0);
    if(msb(sec) != 7) error_print(1);
    if(msb(max1) != 63) error_print(2);
    if(msb(max2) != 63) error_print(3);
    
    printf("\033[36m MSB ENDED !\033[0m\n");

}

void duplicate_testing()
{
    uint64_t dup_res = duplicate(0b1011, 2);
    uint64_t same1 = duplicate(0b1011, 1);
    uint64_t same2 = duplicate(0b1011, 0);
    uint64_t full_1 = duplicate(0b1011, -1);
    uint64_t full_2 = duplicate(0b1011, 9);
    uint64_t full_3 = duplicate(0b1011, 8);

    if(dup_res != 2827) error_print(4);
    if(same1 != 0b1011) error_print(5);
    if(same2 != 0) error_print(6);
    if(full_1 != ONES) error_print(7);
    if(full_2 != ONES) error_print(8);
    if(full_3 != 0x0B0B0B0B0B0B0B0B)   error_print(9);

    printf("\033[36m DUPLICATE ENDED !\033[0m\n");
}

void extractBits_testing()
{
    uint64_t key1 = 0x8001000000000021;
    uint64_t key2 = 0x000111001B001101;
    uint64_t mask1 = 0xBFB08580;            // 63, 48, 5, 0

    if(extractBits(key1,mask1) != 0xF) error_print(10);
    if(extractBits(key2,mask1) != 5) error_print(11);

    


    printf("\033[36m extractingBits ENDED !\033[0m\n");
}

void leftField_testing()
{
    uint64_t packedKeys_1 = 0x8001000000000021;
    uint64_t packedKeys_2 = 0x000111001B001101;
    uint64_t zero = 0ULL;

    if(leftField(packedKeys_1) != 8) error_print(12);
    if(leftField(packedKeys_2) != 7) error_print(13);
    if(leftField(zero) != 0) error_print(14);
    
    printf("\033[36m leftField ENDED !\033[0m\n");
}

// Тесты от gemini
void fill_testing() {
    uint64_t key = 0b10110; // бит на позиции 3 (1-based) это '1'
    uint64_t res = fill(key, 3);
    // Должно заполнить биты 2, 1 единицами -> 0b10111 (23)
    if(res != 0b10000) printf("Error fill 1: expected 23, got %lu\n", res);

    key = 0b11011; // бит на позиции 3 это '0'
    res = fill(key, 3);
    // Должно заполнить биты 2, 1 нулями -> 0b11000 (24)
    if(res != 0b11111) printf("Error fill 2: expected 24, got %lu\n", res);

    key = 0xAAAAAAAAAAAAAAAAULL; // 1010...
    if(fill(key, 65) != key) printf("Error fill 3: pos 65 failed\n");

    printf("\033[36m FILL ENDED !\033[0m\n");
}

void packedRank_testing() {
    // Создаем packedKeys вручную: [10, 20, 30]
    uint64_t pk = (ONES << 24) | (30ULL << 16) | (20ULL << 8) | 10ULL;
    
    // Ищем ранг для 5 (меньше всех) -> должен быть 0
    if(packedRank(pk, 5) != 0) printf("Error packedRank 1: expected 0, bit got: %d\n",packedRank(pk, 5));
    
    // Ищем ранг для 15 (больше 10) -> должен быть 1
    if(packedRank(pk, 15) != 1) printf("Error packedRank 2: expected 1, bit got: %d\n",packedRank(pk, 15));
    
    // Ищем ранг для 25 -> должен быть 2
    if(packedRank(pk, 25) != 2) printf("Error packedRank 3: expected 2, bit got: %d\n",packedRank(pk, 25));
    
    // Ищем ранг для 40 -> должен быть 3
    if(packedRank(pk, 40) != 3) printf("Error packedRank 4: expected 3, bit got: %d\n",packedRank(pk, 40));

    

    printf("\033[36m packedRank ENDED !\033[0m\n");
}

void insertKey_testing() {
    uint64_t pk = ONES;
    pk = insertKey(pk, 10);
    pk = insertKey(pk, 30);
    pk = insertKey(pk, 20); // Вставка между 10 и 30

    uint8_t k0 = pk & 0x7F;
    uint8_t k1 = (pk >> 8) & 0x7F;
    uint8_t k2 = (pk >> 16) & 0x7F;

    if(k0 != 10 || k1 != 20 || k2 != 30) printf("Error insertKey: sequence broken. Got [%d, %d, %d]\n", k0, k1, k2);
        
    

    

    printf("\033[36m insertKey ENDED !\033[0m\n");
}

void fill_testing_2(){
    char buf[65];
    uint64_t keyTest = 0x0000010000000000;
    if(fill(keyTest, 40) != 0x1FFFFFFFFFF)
    {
        printBinOfUINT(fill(keyTest, 40),buf);
        printf("ERROR\nactual result: %s\n",buf);
    }
    keyTest = 0xFFFFFE000BC00000;
    if(fill(keyTest, 40) != 0xFFFFFE0000000000)
    {
        printBinOfUINT(fill(keyTest, 40),buf);
        printf("ERROR\nactual result: %s\n",buf);
    }
    if(fill(keyTest, 65)!=keyTest) printf("error with outbound pos\n");
   
    
    printf("\033[36m fillTesting ENDED !\033[0m\n");
}


void construct_testing() {fusNode node;
    // Инициализируем тестовые данные (3 ключа)
    node.keys[0] = 10;   // 0000 1010
    node.keys[1] = 20;   // 0001 0100 -> различие в 4-м бите
    node.keys[2] = 30;   // 0001 1110 -> различие в 3-м бите
    node.amount = 3;

    // Вызываем вашу функцию
    construct(&node);

    // 1. Проверяем packedImportantBits
    // Должны быть записаны индексы 4 и 3 с сигнальными битами (0x84 и 0x83)
    uint8_t b0 = node.packedImportantBits & 0xFF;
    uint8_t b1 = (node.packedImportantBits >> 8) & 0xFF;
    
    
    ASSERT_TEST(b0 == 0x84 || b0 == 0x83, "Important bit 0 missing");
    ASSERT_TEST(b1 == 0x84 || b1 == 0x83, "Important bit 1 missing");

    // 2. Проверяем packedKeys (структура: [ONES | Sketch2 | Sketch1 | Sketch0])
    // Эскизы для 10, 20, 30 по битам 4 и 3:
    // 10 (01010) -> биты [4]=0, [3]=1 -> эскиз 01 (1)
    // 20 (10100) -> биты [4]=1, [3]=0 -> эскиз 10 (2)
    // 30 (11110) -> биты [4]=1, [3]=1 -> эскиз 11 (3)
    
    uint8_t s0 = node.packedKeys & 0x7F;
    uint8_t s1 = (node.packedKeys >> 8) & 0x7F;
    uint8_t s2 = (node.packedKeys >> 16) & 0x7F;

    char buf[65];
    printBinOfUINT(node.packedKeys,buf);
    
    ASSERT_TEST(s0 == 1, "Sketch 0 incorrect");
    ASSERT_TEST(s1 == 2, "Sketch 1 incorrect");
    ASSERT_TEST(s2 == 3, "Sketch 2 incorrect");

    // 3. Проверка того, что "хвост" забит единицами
    // Для 3 ключей (24 бита), биты с 24 по 63 должны быть 1
    uint64_t tail = node.packedKeys >> 24;
    uint64_t expected_tail = ONES >> 24;
    ASSERT_TEST(tail == expected_tail, "Leftmost bits are not 1");

    printf("\033[36m CONSTRUCT TESTING ENDED !\033[0m\n");
}

void rank_comprehensive_testing() {
    fusNode node;
    // Подготовка данных: ключи с интересными битовыми пересечениями
    // 10: 0000 1010
    // 20: 0001 0100
    // 30: 0001 1110
    node.keys[0] = 10;
    node.keys[1] = 20;
    node.keys[2] = 30;
    //node.keys[3] = 40;

    //node.isLeaf = 1;

    //for(int i = 0; i < 4 ; i++)
    //{
    //    node.childs[i] = NULL
    //}
    node.amount = 3;

    // Строим внутренние константы узла
    construct(&node);

    

    
    
    // Сценарий 1: Ключ меньше первого (проверка i=0)
    // Ранг должен быть 0
    uint8_t r1 = rank(5, &node);
    //printf("r1: %d\n",r1);
    ASSERT_TEST(r1 == 0, "Rank test 1 failed: key < keys[0]");

    // Сценарий 2: Ключ совпадает с существующим
    // Ранг 20 должен быть 1
    uint8_t r2 = rank(20, &node);
    //printf("r2: %d\n",r2);
    ASSERT_TEST(r2 == 2, "Rank test 2 failed: key == keys[1]");

    // Сценарий 3: Ключ между 20 и 30
    // 25: 0001 1001. Ранг должен быть 2
    uint8_t r3 = rank(25, &node);
    //printf("r3: %d\n",r3);
    ASSERT_TEST(r3 == 2, "Rank test 3 failed: keys[1] < key < keys[2]");

    // Сценарий 4: Ключ больше последнего
    // 100: ранг должен быть 3
    uint8_t r4 = rank(100, &node);
    //printf("r4: %d\n",r4);
    ASSERT_TEST(r4 == 3, "Rank test 4 failed: key > keys[2]");
    


    // Сценарий 5: Хитрая проверка префикса (D.4 - D.6)
    // Возьмем число, которое по "важным битам" похоже на 30, но само меньше него.
    // Если важные биты 4 и 3, то у 30 это '11'.
    // Число 24 (0001 1000) тоже имеет '11' в этих битах.
    // Алгоритм должен понять через DiffPtr и Fill, что 24 < 30 и ранг равен 2.
    uint8_t r5 = rank(24, &node);
    ASSERT_TEST(r5 == 2, "Rank test 5 failed: prefix collision handling");
    //printf("r5: %d\n",r5);
    printf("\033[36m RANK COMPREHENSIVE TESTING ENDED !\033[0m\n");
    
    
}

void rank_stress_testing() {
    fusNode node;
    // Сценарий А: Ключи с одинаковыми "важными битами"
    // Допустим, важные биты - это 0 и 1.
    // У всех этих чисел биты 0 и 1 одинаковы (равны 0), но сами числа разные.
    node.keys[0] = 0b0100; // 4
    node.keys[1] = 0b1000; // 8
    node.keys[2] = 0b1100; // 12
    node.amount = 3;
    construct(&node);

    ASSERT_TEST(rank(2, &node) == 0, "Stress 1: Key smaller than all (bit collision)");
    ASSERT_TEST(rank(6, &node) == 1, "Stress 2: Key between keys[0] and keys[1]");
    ASSERT_TEST(rank(14, &node) == 3, "Stress 3: Key larger than all");

    // Сценарий Б: Граничные 64-битные значения
    node.keys[0] = 0x0000000000000001ULL;
    node.keys[1] = 0x7FFFFFFFFFFFFFFFULL;
    node.keys[2] = 0xFFFFFFFFFFFFFFFEULL;
    node.amount = 3;
    construct(&node);

    ASSERT_TEST(rank(0, &node) == 0, "Edge 1: Zero key");
    ASSERT_TEST(rank(0x8000000000000000ULL, &node) == 2, "Edge 2: Mid-range large key");
    ASSERT_TEST(rank(0xFFFFFFFFFFFFFFFFULL, &node) == 3, "Edge 3: Max uint64 key");

    // Сценарий В: "Сендвич" (очень близкие значения)
    node.keys[0] = 1000;
    node.keys[1] = 1001;
    node.keys[2] = 1002;
    node.amount = 3;
    construct(&node);
    
    ASSERT_TEST(rank(1001, &node) == 2, "Sandwich 1: Exact match in middle");
    ASSERT_TEST(rank(1003, &node) == 3, "Sandwich 2: Just above max");

    printf("\033[36m STRESS TESTS COMPLETED ! \033[0m\n");
}

void run_btree_tests() {
    fusNode* root = NULL;

    printf("--- Starting B-Tree Fusion Tests ---\n");

    // ТЕСТ 1: Массовая вставка (Проверка Split)
    // Вставляем 20 элементов. При факторе 8 дерево должно разделиться минимум дважды.
    for (uint64_t i = 1; i <= 20; i++) {
        bTreeInsert(&root, i * 10, (int)(i * 100));
        printf("\033[32I=%lu\033[0m\n", i);
        printTree(root,0,0);
    }
    printf("Test 1: Insertion of 20 elements completed.\n");

    // ТЕСТ 2: Поиск (Проверка Lookup и Rank)
    fusNode* found = lookup(root, 50);
    printTree(root,0,0);
    assert(found != NULL);
    // Находим индекс ключа внутри узла для проверки значения
    uint8_t rk = rank(50, found);
    assert(found->values[rk-1] == 500);
    
    assert(lookup(root, 999) == NULL); // Несуществующий
    printf("Test 2: Lookup and value validation passed.\n");

    // ТЕСТ 3: Удаление с заимствованием (Borrow)
    // Удалим ключ, чтобы спровоцировать балансировку
    bTreeDelete(&root, 10); 
    assert(lookup(root, 10) == NULL);
    printf("Test 3: Deletion with borrow/rebalance passed.\n");

    // ТЕСТ 4: Массовое удаление (Проверка Merge и уменьшения высоты)
    for (uint64_t i = 2; i <= 20; i++) {
        bTreeDelete(&root, i * 10);
    }
    assert(root == NULL || (root->amount == 0 && root->isLeaf == 1));
    printf("Test 4: Mass deletion (tree cleanup) passed.\n");

    // ТЕСТ 5: Граничные значения
    bTreeInsert(&root, 0, 0);
    bTreeInsert(&root, 0xFFFFFFFFFFFFFFFFULL, -1);
    assert(lookup(root, 0) != NULL);
    printf("Test 5: Edge cases (0 and MaxUint64) passed.\n");

    freeBTree(&root);
    printf("--- All B-Tree Tests Passed! ---\n");


void run_all_tests(){
    msb_testing();
    duplicate_testing();
    extractBits_testing();
    leftField_testing();
    
    //God bless this code
    fill_testing();
    packedRank_testing();
    insertKey_testing();
    fill_testing_2();
    construct_testing();
    rank_comprehensive_testing();
    rank_stress_testing();

    run_btree_tests();
}

int main(){

    uint64_t zero = 0;
    uint64_t one = 1ULL;

    printf("zero: %lu, ~zero: %lu, \n one: %lu, ~one: %lu\n",
        zero, -zero, one, -one);

    return 0;
}