slab: slab.c
	gcc -Wall -Wextra -g -o build/slab slab.c

unit: tests/unit_tests.c slab.c common/bump.c
	gcc -Wall -Wextra -g -DTESTING -o build/unit_tests slab.c common/bump.c tests/unit_tests.c


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
