CFLAGS = -Wall -Werror -Wextra 
CPPFLAGS = -MMD -I includes/ 
LDFLAGS = -lm

# Пути к файлам
BUILD_DIR = build
SRC_DIR = src
TEST_DIR = tests

.PHONY: all clean test

all: fusionDSA

# --- Основная программа ---
fusionDSA: $(BUILD_DIR)/main.o $(BUILD_DIR)/libfusion.a 
	gcc $(CFLAGS) -O2 $^ -o $@ $(LDFLAGS)

# --- Правило для тестов ---
# Компилирует test.c и линкует с библиотекой
test: $(TEST_DIR)/test_runner
	

$(TEST_DIR)/test_runner: $(BUILD_DIR)/test.o $(BUILD_DIR)/libfusion.a
	gcc $(CFLAGS) $^ -o $@ $(LDFLAGS)

rbt: RBT
	

RBT: $(BUILD_DIR)/main.o $(BUILD_DIR)/librbtree.a
	gcc $(CFLAGS) $^ -o $@ $(LDFLAGS)

# --- Компиляция объектов ---
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c | $(BUILD_DIR)
	gcc -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(BUILD_DIR)/fusion.o: $(SRC_DIR)/fusion.c | $(BUILD_DIR)
	gcc -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(BUILD_DIR)/rbtree.o: $(SRC_DIR)/rbtree.c | $(BUILD_DIR)
	gcc -c $(CFLAGS) $(CPPFLAGS) $< -o $@	

$(BUILD_DIR)/test.o: $(TEST_DIR)/test.c | $(BUILD_DIR)
	gcc -c $(CFLAGS) $(CPPFLAGS) $< -o $@

# --- Статическая библиотека ---
$(BUILD_DIR)/libfusion.a: $(BUILD_DIR)/fusion.o
	ar rcs $@ $^

$(BUILD_DIR)/librbtree.a: $(BUILD_DIR)/rbtree.o
	ar rcs $@ $^

# Создание папки build, если её нет
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Автоматическое включение зависимостей (.d файлы)
-include $(BUILD_DIR)/*.d

clean:
	rm -rf $(BUILD_DIR) fusionDSA

