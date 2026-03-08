slab: slab.c
	gcc -Wall -Wextra -g -o build/slab slab.c

unit: tests/unit_tests.c allocator.c
	gcc -Wall -Wextra -g -o build/unit_tests slab.c tests/unit_tests.c


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
