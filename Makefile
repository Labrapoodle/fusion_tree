CFLAGS = -Wall -Werror -Wextra 
CPPFLAGS =  -MMD -I includes/ 

.PHONY: all
all: fusionDSA

-include main.d minheap.d fibheap.d

fusionDSA: build/main.o build/libminheap.a build/fibheap.a
	gcc -Iincludes/ $(CFLAGS) $^ -o $@ -lm
	rm build/main.d
	rm build/main.o
	rm build/minheap.o
	rm build/minheap.d
	rm build/fibheap.o
	rm build/fibheap.d

build/main.o: src/main.c
	gcc -c $(CFLAGS) $(CPPFLAGS)  $< -o $@

build/libminheap.a: build/minheap.o
	ar rcs $@ $^

build/minheap.o: src/minheap.c
	gcc -c $(CPPFLAGS) $< -o $@

build/fibheap.a: build/fibheap.o
	ar rcs $@ $^

build/fibheap.o: src/fibheap.c
	gcc -c $(CPPFLAGS)  $< -o $@