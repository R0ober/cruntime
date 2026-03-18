# make slab BUMP_DEFINE=-DUSE_EMBEDDED
BUMP_DEFINE ?=

slab: slab.c common/bump.c
	mkdir -p build
	gcc -Wall -Wextra -g $(BUMP_DEFINE) -o build/slab slab.c common/bump.c visual.c 

unit: tests/unit_tests.c slab.c common/bump.c gc.c
	gcc -Wall -Wextra -g -DTESTING -o build/unit_tests slab.c common/bump.c gc.c visual.c tests/unit_tests.c

clean:
	rm -f build/slab build/slab.o build/bump.o build/unit_tests build/unit_embedded_tests

unit_embedded:
	mkdir -p build
	gcc -Wall -Wextra -g -DTESTING -DUSE_EMBEDDED -DALLOCATOR_HEAP_SIZE=262144 -o build/unit_embedded_tests slab.c common/bump.c gc.c visual.c tests/unit_tests.c
	./build/unit_embedded_tests

# ===============================================================
# insperation 
# CC = gcc
# CFLAGS = -Wall -Wextra -g
# TARGET = my_malloc
#
# all: $(TARGET)
#
# $(TARGET): malloc.c more.c
# 	$(CC) $(CFLAGS) -o $(TARGET) malloc.c more.c
#
# clean:
# 	rm -f $(TARGET)
# ===============================================================
