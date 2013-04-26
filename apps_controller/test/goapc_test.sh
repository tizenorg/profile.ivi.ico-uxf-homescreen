#!/bin/sh

export LD_LIBRARY_PATH=../ico-app-framework/.libs:$LD_LIBRARY_PATH
gdb ./apc_test

