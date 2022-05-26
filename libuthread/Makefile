CC := gcc
LIB := ar rcs
CFLAGS := -Wall -Wextra -Werror
aobjs := queue.o uthread.o preempt.o context.o
targets := libuthread.a

ifneq ($(V), 1)
Q = @
endif

all: $(targets)

deps := $(patsubst %.o,%.d,$(aobjs))
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)

libuthread.a: $(aobjs)
	@echo "LIB $@"
	$(Q)$(LIB) $@ $^

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)

clean:
	@echo "CLEAN"
	$(Q)rm -f $(targets) $(aobjs) $(deps)
