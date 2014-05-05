#!/bin/sh
gcc -fpreprocessed -dD -E -P annotated.c > tinywm.c
