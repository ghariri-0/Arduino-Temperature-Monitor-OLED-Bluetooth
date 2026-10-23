#!/bin/sh
# Host test: compiles the real sketch with stub libraries and a scripted DS18B20,
# then checks min/max/history behaviour. Needs only g++.   Usage: sh tests/host/run_tests.sh
set -e
cd "$(dirname "$0")"
{ echo '#include "Arduino.h"'; sed 's/^#include <\(.*\)>/#include "\1"/' ../../temperature_monitor/temperature_monitor.ino; } > sketch.cpp
g++ -std=c++17 -w -Istubs harness.cpp u8g2_stub.cpp -o host_test
fail=0
check() { # name, expected substring, readings...
  name=$1; exp=$2; shift 2
  got=$(./host_test "$@")
  if echo "$got" | grep -q "BAD LINE"; then echo "FAIL $name: graph line outside its frame"; fail=1;
  elif echo "$got" | grep -q "$exp"; then echo "PASS $name"; else echo "FAIL $name: expected '$exp'"; echo "$got" | tail -1; fail=1; fi
}
check "normal readings"                "MIN=24.50 MAX=24.70" 24.5 24.6 24.7
check "85.00 power-on value at boot"   "MIN=24.60 MAX=24.70" 85 24.6 24.7
check "probe unplugged and re-plugged" "MIN=24.50 MAX=24.70" 24.5 -127 -127 85 24.6 24.7
check "real 85 C still accepted"       "MAX=85.00"           84.9 -127 85 85 85
check "full range, 64-point history"   "MIN=-55.00 MAX=125.00 HIST=64" $(i=0; while [ $i -lt 100 ]; do printf -- '-55 '; i=$((i+1)); done) $(i=0; while [ $i -lt 100 ]; do printf '125 '; i=$((i+1)); done)
rm -f sketch.cpp host_test
exit $fail
