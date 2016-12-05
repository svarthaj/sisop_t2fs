FILES := logging utils disk t2fs inode
TESTS := utils disk t2fs inode

OBJECTS := lib/apidisk.o lib/bitmap2.o $(patsubst %,bin/%.o,$(FILES))
TESTS := $(patsubst %,teste/test_%.out,$(TESTS))

CC := gcc -Wall -std=c99 -I ./include

.PHONY: all
all:

.PHONY: test
test: $(TESTS)

teste/test_%.out: restore_disk $(OBJECTS) teste/test_%.c
	$(CC) $^ -o $@
	./$@ && rm $@

.PHONY: restore_disk
restore_disk:
	cp t2fs_disk.dat.bak t2fs_disk.dat

bin/%.o: src/%.c
	$(CC) -c $^ -o $@

.PHONY: clean
clean:
	rm -rf lib/*.a bin/*.o src/*~ include/*~ teste/*.out *~
