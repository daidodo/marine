#!/bin/sh

if [ $# -lt 1 ] ; then
  echo "Usage: $0 EXE [EXE2 ...]"
  exit 1
fi

PWD=`pwd`

kill_server()
{
  BIN="$1"
  CMD=${PWD}/$1

  PS_RECORD=`ps -ef | grep -v grep | grep "${CMD}"`
  PS_PID=`echo "${PS_RECORD}" | awk '{print $2}'`

  if [ "${PS_RECORD}" = "" ]; then
    echo "# ${BIN} is NOT running"
  else
    kill ${PS_PID}

    sleep 1

    PS_CHECK=`ps -ef | grep -v grep | grep "${CMD}"`
    if [ "${PS_CHECK}" = "" ]; then
      echo "# ${BIN} stopped"
      echo "${PS_RECORD}"
    else
      echo "# killing ${BIN} FAILED"
      echo "${PS_CHECK}"
      exit 1
    fi
  fi
}

for BIN in "$@" ; do
  kill_server $BIN
done
