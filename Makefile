PREFIX ?= /usr/local

make:
	gcc -std=c99 -Wall -o zpm -g zpm.c

test:
	tests/setup.sh
	ZPM_TEST_REPOS=/tmp/zpm-test-suite/repositories cram -v tests/*.t -v
	
install: make
	cp zpm ${PREFIX}/bin/zpm
	chmod u+x ${PREFIX}/bin/zpm

uninstall:
	rm ${PREFIX}/bin/zpm

