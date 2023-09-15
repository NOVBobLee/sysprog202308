CFLAGS := -std=c11 -Wall -g -O2 -fsanitize=thread
LDFLAGS := -lpthread

INC := ..
_HEADERS := atomic.h futex.h mutex.h spinlock.h
HEADERS := $(addprefix $(INC)/,$(_HEADERS))
CFLAGS += -I$(INC)

ALL := perf_pthread perf_linux


.PHONY: all clean
all: $(ALL)

perf_%: perf.c $(HEADERS)
	$(CC) $(CFLAGS) perf.c -o $@ $(LDFLAGS)

perf_pthread: CFLAGS += -DUSE_PTHREADS
perf_linux: CFLAGS += -DUSE_LINUX

perf: $(ALL)
	@$(foreach t,$^,\
	    printf "Running $(t) ...\n"; \
	    sudo sh -c "taskset -c 0,1 ./$(t)" \
	)

clean:
	$(RM) $(ALL)
