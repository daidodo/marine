#!/bin/bash

printUsage()
{
  echo "Usage: $0 EXE VNODE [HASH]"
}

if [ $# -lt 2 ] ; then
  printUsage
  exit 1
fi

EXE="$1"
VNODE="$2"
HASH="$3"

for (( i = 1000;;i += 1000 )) ; do
  echo -n $i
  ./$EXE $i $VNODE $HASH
done
  
