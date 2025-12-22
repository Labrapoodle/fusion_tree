CFLAGS = -Wall -Werror -Wextra 
CPPFLAGS =  -MMD -I includes/ 

.PHONY: all
all: fusionDSA

-include main.d minheap.d fibheap.d

fusionDSA: build/main.o build/libfusion.a 
	gcc -Iincludes/ $(CFLAGS) $^ -o $@ -lm
	rm build/main.d
	rm build/main.o
	rm build/fusion.o
	rm build/fusion.d
	

build/main.o: src/main.c
	gcc -c $(CFLAGS) $(CPPFLAGS)  $< -o $@

build/libfusion.a: build/fusion.o
	ar rcs $@ $^

build/fusion.o: src/fusion.c
	gcc -c $(CPPFLAGS) $< -o $@

