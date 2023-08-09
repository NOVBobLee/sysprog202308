CC := gcc
STD := -std=c11
CFLAGS = $(STD) -Wall -Wextra
#CFLAGS += -g -fsanitize=address
CFLAGS += -g -fsanitize=thread

LDFLAGS =
LDFLAGS += -lpthread

TARGET := qsort_mt

.PHONY: all clean format
all: $(TARGET)

$(TARGET): %: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test: $(TARGET)
	./$(TARGET) -tv -f2 -h2 -n100

format: $(TARGET).c
	clang-format -i -style=file:../.clang-format $(TARGET).c

clean:
	$(RM) $(TARGET)
