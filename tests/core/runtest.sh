#!/bin/sh

#
# By default the glibc __libc_message() writes to /dev/tty before calling
# the abort(). Exporting this macro makes it to use stderr instead.
#
# The main usage of the function are malloc assertions, so this makes us catch
# the malloc error message by catching stderr output.
#
export LIBC_FATAL_STDERR_=1

TEST="$1"
shift

LD_PRELOAD=`pwd`/../framework/libtst_preload.so LD_LIBRARY_PATH=../../build/ gdb "./$TEST" "$@"
