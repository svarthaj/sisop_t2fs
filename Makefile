FILES := logging utils disk t2fs inode list
TESTS := utils disk t2fs inode list

OBJECTS := lib/apidisk.o lib/bitmap2.o $(patsubst %,bin/%.o,$(FILES))
TESTS := $(patsubst %,teste/test_%.out,$(TESTS))

CC := gcc -Wall -std=c99 -I ./include

all:

test: $(TESTS)

teste/test_%.out: $(OBJECTS) teste/test_%.c
	$(CC) $^ -o $@
	./$@ && rm $@

bin/%.o: src/%.c
	$(CC) -c $^ -o $@

clean:
	rm -rf lib/*.a bin/*.o src/*~ include/*~ teste/*.out *~
