#!/bin/bash

printUsage()
{
  echo "Usage: $0 exe"
}

if [ $# -lt 1 ] ; then
  printUsage
  exit 1
fi

EXE="$1"

for (( i = 100;;i += 100 )) ; do
  echo -n $i
  ./$EXE $i
done
