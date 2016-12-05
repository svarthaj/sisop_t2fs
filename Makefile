FILES := logging utils disk t2fs inode list open_file
TESTS := utils disk t2fs inode list open_file

OBJECTS := lib/apidisk.o lib/bitmap2.o $(patsubst %,bin/%.o,$(FILES))
TESTS := $(patsubst %,teste/test_%.out,$(TESTS))

CC := gcc -Wall -std=c99 -I ./include

.PHONY: all test restore_disk clean
all:

test: $(TESTS)

teste/test_%.out: $(OBJECTS) teste/test_%.c
	cp t2fs_disk.dat.bak t2fs_disk.dat
	$(CC) $^ -o $@
	./$@ && rm $@


bin/%.o: src/%.c
	$(CC) -c $^ -o $@

clean:
	rm -rf lib/*.a bin/*.o src/*~ include/*~ teste/*.out *~
