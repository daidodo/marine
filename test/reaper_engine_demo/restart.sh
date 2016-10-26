#!/bin/bash

if [ $# -lt 1 ] ; then
  echo "Usage: $0 EXE [EXE2 ...]"
  exit 1
fi

./kill.sh "$@"

sleep 2

RET=-1
./start.sh "$@" && RET=0

exit $RET
