CFLAGS := -std=c11 -Wall -g -O2 -D_GNU_SOURCE -fsanitize=thread
LDFLAGS := -lpthread

INC := ..
_HEADERS := atomic.h futex.h mutex.h spinlock.h
HEADERS := $(addprefix $(INC)/,$(_HEADERS))
CFLAGS += -I$(INC)

CFLAGS += -DUSE_PI

ALL := test_pthread test_linux


.PHONY: all clean
all: $(ALL)

test_%: main.c $(HEADERS)
	$(CC) $(CFLAGS) main.c -o $@ $(LDFLAGS)

test_pthread: CFLAGS += -DUSE_PTHREADS
test_linux: CFLAGS += -DUSE_LINUX

check: $(ALL)
	@$(foreach t,$^,\
	    printf "Running $(t) ...\n"; \
	    sudo sh -c "taskset -c 0 ./$(t)" \
	)

clean:
	$(RM) $(ALL)
