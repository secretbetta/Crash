# Output binary name
bin=crash

# Set the following to '0' to disable log messages:
LOGGER ?= 1

# Compiler/linker flags
CFLAGS += -g -Wall -lm -lreadline -fPIC -DLOGGER=$(LOGGER)
LDFLAGS +=

src=history.c shell.c ui.c util.c builtin.c
obj=$(src:.c=.o)

all: $(bin) libshell.so

$(bin): $(obj)
	$(CC) $(CFLAGS) $(LDFLAGS) $(obj) -o $@

libshell.so: $(obj)
	$(CC) $(CFLAGS) $(LDFLAGS) $(obj) -shared -o $@

shell.o: shell.c history.h logger.h ui.h builtin.h
history.o: history.c history.h logger.h
ui.o: ui.h ui.c logger.h history.h util.h builtin.h
util.o: util.c util.h logger.h
builtin.o: builtin.c builtin.h history.h logger.h util.h

clean:
	rm -f $(bin) $(obj) libshell.so vgcore.*


# Tests --

test: $(bin) libshell.so ./tests/run_tests
	@DEBUG="$(debug)" ./tests/run_tests $(run)

testupdate: testclean test

./tests/run_tests:
	rm -rf tests
	git clone https://github.com/USF-OS/Shell-Testcases.git tests

testclean:
	rm -rf tests
