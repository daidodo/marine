#!/bin/sh

if [ $# -lt 1 ] ; then
  echo "Usage: $0 EXE [EXE2 ...]"
  exit 1
fi

PWD=`pwd`

start_server()
{
  BIN="$1"
  CMD="${PWD}/${BIN}"
  PS_RECORD=`ps -ef | grep -v grep | grep "${CMD}"`

  if [ "${PS_RECORD}" != "" ]; then
    echo "# ${BIN} is RUNNING"
    echo "${PS_RECORD}"
  else
    export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:../lib/
    ulimit -n 2000
    ulimit -s unlimited
    ulimit -c unlimited
    #memory leak check
    #valgrind --tool=memcheck --leak-check=full ${CMD} 1>/dev/null 2>memcheck.txt &
    #thread synchronisation check
    #valgrind --tool=helgrind --trace-level=1 ${CMD} 1>/dev/null 2>syncheck.txt &
    #run program
    ${CMD} &

    sleep 1

    PS_CHECK=`ps -ef | grep -v grep | grep "${CMD}"`
    if [ "${PS_CHECK}" = "" ]; then
      echo "# starting ${BIN} FAILED"
      exit 1
    else
      echo "# ${BIN} started"
      echo "${PS_CHECK}"
    fi
  fi
}

for BIN in "$@" ; do
  start_server $BIN
done
