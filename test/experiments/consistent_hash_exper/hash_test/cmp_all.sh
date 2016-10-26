#!/bin/bash

printUsage()
{
  echo "Usage: $0 NODE VNODE [-t]"
  echo "    NODE    node count"
  echo "    VNODE   vnode count per node"
  echo "    -t      time stats"
}

ts=0

for a in $* ; do
  case "$a" in
    "-t" )
      ts=1
      ;;
    * )
      if [ "$node" = "" ] ; then
        node="$a"
      elif [ "$vnode" = "" ] ; then
        vnode="$a"
      else
        printUsage
        exit 1
      fi
      ;;
  esac
done
        
#echo "ts=$ts, node=$node, vnode=$vnode"
      
if [ "$node" = "" ] ; then
  printUsage
  exit 1
fi

if [ "$vnode" = "" ] ; then
  printUsage
  exit 1
fi

echo "[$node, $vnode]"
for exe in *_test ; do
  echo -n "run $exe:"
  if [ $ts -eq 0 ] ; then
    if ! ./$exe $node $vnode ; then
      exit 1
    fi
  else
    if ! time ./$exe $node $vnode ; then
      exit 1
    fi
  fi
done
