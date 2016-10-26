#!/bin/bash

if [ $# -lt 1 ] ; then
  echo "$0 test1 [test2 ...]"
  exit 1
fi

echo "----TEST begin----"

#ulimit -c unlimited

for test in "$@" ; do
  if ! ./$test ; then
    echo "$test failed----"
    exit 1
  fi
done

echo "----TEST finished----"
