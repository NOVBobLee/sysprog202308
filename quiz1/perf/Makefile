CFLAGS := -std=c11 -Wall -O2
LDFLAGS := -lpthread

CFLAGS += -g -fsanitize=thread

INC := ..
_HEADERS := atomic.h futex.h mutex.h spinlock.h
HEADERS := $(addprefix $(INC)/,$(_HEADERS))
CFLAGS += -I$(INC)

ALL := perf_pthread perf_linux test_cache_ld test_cache_cas


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

test_cache_%: test_cache.c ../atomic.h ../spinlock.h
	$(CC) $(CFLAGS) test_cache.c -o $@ $(LDFLAGS)

test_cache_ld: CFLAGS += -DTEST_LD
test_cache_cas: CFLAGS += -DTEST_CAS

clean:
	$(RM) $(ALL)
