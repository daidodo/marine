#!/bin/bash

if [ $# -lt 1 ] ; then
  echo "Usage: $0 DIR1 [DIR2 ...]"
  exit 1
fi

for d in $* ; do
  for f in $d/*.h ; do
    echo "$f -> ${f}h"
  done
done

echo
read -p "confirm to process(y/n)?" a
if [ "$a" = "y" ] ; then
  for d in $* ; do
    for f in $d/*.h ; do
      svn mv $f ${f}h
    done
  done
fi
