CC := gcc
STD := -std=gnu11
CFLAGS = $(STD) -Wall
#CFLAGS += -g -fsanitize=address
#CFLAGS += -g -fsanitize=thread

LDFLAGS =
LDFLAGS += -lpthread

SRC := align_up.c qsort_mt.c s_tree.c
BIN := $(SRC:%.c=%)

.PHONY: all clean format
all: $(BIN)

align_up: %: %.c
	$(CC) $(CFLAGS) -o $@ $^

qsort_mt: %: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

s_tree: %: %.c
	$(CC) $(CFLAGS) -o $@ $^

test-align_up: align_up
	./$<

test-qsort_mt: qsort_mt
	./$< -tv -f2 -h2 -n100

test-s_tree: s_tree
	./$<

format:
	clang-format -i -style=file:../.clang-format $(SRC)

clean:
	$(RM) $(BIN)
