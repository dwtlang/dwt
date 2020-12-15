#!/bin/sh

g++ -O3 fib.c -ofib
/usr/bin/time -f "C,%e" ./fib 2> results.csv 1>/dev/null
/usr/bin/time -f "Dwt,%e" dwt fib.dwt >> results.csv 2>&1
/usr/bin/time -f "Wren,%e" wren_test fib.wren >> results.csv 2>&1
/usr/bin/time -f "LuaJIT (off),%e" luajit -joff fib.lua >> results.csv 2>&1
/usr/bin/time -f "LuaJIT (on),%e" luajit fib.lua >> results.csv 2>&1
/usr/bin/time -f "Lua,%e" lua fib.lua >> results.csv 2>&1
/usr/bin/time -f "Ruby,%e" ruby fib.rb >> results.csv 2>&1
/usr/bin/time -f "Python,%e" python fib.py >> results.csv 2>&1

python plot.py

