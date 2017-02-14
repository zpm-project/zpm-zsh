#!/bin/sh

[ "$#" -eq 0 ] && echo "'$0' needs a least one argument." && exit 1
trap profiling_clean INT TERM

LOG_FILE="valgrind-$$.log"
VALGRIND_OPTS="--log-file=$LOG_FILE"


valgrind --leak-check=yes "$VALGRIND_OPTS" "$@" 2>&1
RET="$?"
grep "0 errors from 0 contexts" "$LOG_FILE" > /dev/null
[ "$?" -eq 1 ] && cat "$LOG_FILE"
rm "$LOG_FILE"
exit "$RET"
