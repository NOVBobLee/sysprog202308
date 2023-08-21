CC := gcc
STD := -std=gnu11
CFLAGS = $(STD) -Wall
#CFLAGS += -g -fsanitize=thread
#CFLAGS += -g -fsanitize=address
LDFLAGS := -lpthread

.PHONY: all clean test-work-steal test-mpmc
all: work-steal mpmc

work-steal: %: %.c
	$(CC) $(CFLAGS) -O2 -o $@ $^ $(LDFLAGS)

mpmc: %: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test-work-steal: work-steal
	./$<

test-mpmc: mpmc
	./$<

clean:
	$(RM) work-steal mpmc
