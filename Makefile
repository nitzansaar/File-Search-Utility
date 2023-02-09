# Output binary name
bin=search
lib=search.so

# Set the following to '0' to disable log messages:
LOGGER ?= 1

# Compiler/linker flags
CFLAGS += -g -Wall -fPIC -DLOGGER=$(LOGGER)
LDLIBS +=
LDFLAGS += -L. -Wl,-rpath='$$ORIGIN'

# Source C files
src=search.c
obj=$(src:.c=.o)

# Makefile recipes --
all: $(bin) $(lib)

$(bin): $(obj)
	$(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(obj) -o $@

$(lib): $(obj)
	$(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(obj) -shared -o $@

docs: Doxyfile
	doxygen

clean:
	rm -f $(bin) $(obj) $(lib)
	rm -rf docs outputs

# Individual dependencies --
search.o: search.c logger.h

# Tests --

test_repo=usf-cs521-sp23/P1-Tests

test: $(bin) $(lib) ./.testlib/run_tests ./tests
	@DEBUG="$(debug)" ./.testlib/run_tests $(run)

grade: ./.testlib/grade
	./.testlib/grade $(run)

testupdate: testclean test

testclean:
	rm -rf tests .testlib

./tests:
	rm -rf ./tests
	git clone https://github.com/$(test_repo) tests

./.testlib/run_tests:
	rm -rf ./.testlib
	git clone https://github.com/malensek/cowtest.git .testlib

./.testlib/grade: ./.testlib/run_tests
