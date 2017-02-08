#!/bin/sh

[ "$#" -eq 0 ] && echo "'$0' needs a least one argument." && exit 1

trap profiling_clean INT TERM


PASS_STRING="All heap blocks were freed -- no leaks are possible"
LOG_FILE="valgrind-$$.log"

profiling_clean() {
	rm "$LOG_FILE"
}


if [ -t 1 ]
then VALGRIND_OPTS="--leak-check=yes"
else VALGRIND_OPTS="--log-file=$LOG_FILE"
fi

valgrind "$VALGRIND_OPTS" "$@" 2>&1

RET="$?"

if [ ! -t 0 ]
then
	grep "$PASS_STRING" "$LOG_FILE" > /dev/null || echo "memory test failed (leak)"
	grep "invalid" "$LOG_FILE" > /dev/null && echo "memory test failed (invalid read/write)"
	profiling_clean
fi

exit "$RET"
