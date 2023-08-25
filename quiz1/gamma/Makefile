CC := gcc
STD := -std=gnu11
INC := -I..
CFLAGS = $(STD) -Wall $(INC)
#CFLAGS += -g -fsanitize=address
#CFLAGS += -g -fsanitize=thread
LDFLAGS := -lpthread


BIN := test_linux

.PHONY: all clean check
all: $(BIN)

test_%: qsort_mt.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_linux: CFLAGS += -DUSE_LINUX

check: $(BIN)
	./$< -tv -f2 -h2 -n100

clean:
	$(RM) $(BIN)
