PREFIX ?= /usr/local

CFLAGS =-Wall -g -std=c99 -D_POSIX_C_SOURCE=2

make:
	${CC} ${CFLAGS} -o zpm zpm.c

test: make
	tests/setup.sh
	ZPM="${PWD}/zpm"  ZPM_TEST_REPOS=/tmp/zpm-test-suite/repositories cram -v tests/*.t

valgrind: make
	tests/setup.sh
	ZPM="bash ${PWD}/tests/valgrind_profiling.sh ${PWD}/zpm" ZPM_TEST_REPOS=/tmp/zpm-test-suite/repositories cram -v tests/*.t

install: make
	cp zpm ${PREFIX}/bin/zpm
	chmod u+x ${PREFIX}/bin/zpm

uninstall:
	rm ${PREFIX}/bin/zpm

clean:
	rm zpm
