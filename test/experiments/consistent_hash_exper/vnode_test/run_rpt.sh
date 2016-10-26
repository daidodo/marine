#!/bin/bash

printUsage()
{
  echo "Usage: $0 exe NODE"
}

if [ $# -lt 2 ] ; then
  printUsage
  exit 1
fi

EXE="$1"
NODE="$2"

while true ; do
  ./$EXE $NODE
done
