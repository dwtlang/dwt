#!/bin/sh

g++ -O3 fib.c -ofib
/usr/bin/time -f "c,%e" ./fib 2> results.csv 1>/dev/null
/usr/bin/time -f "dwt,%e" dwt fib.dwt >> results.csv 2>&1
/usr/bin/time -f "luajit,%e" luajit fib.lua >> results.csv 2>&1
/usr/bin/time -f "lua,%e" lua fib.lua >> results.csv 2>&1
/usr/bin/time -f "ruby,%e" ruby fib.rb >> results.csv 2>&1
/usr/bin/time -f "python,%e" python fib.py >> results.csv 2>&1

python plot.py

