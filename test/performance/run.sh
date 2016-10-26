#!/bin/bash

#configs
CONFIG_TEST_BRANCH_MISS=0
CONFIG_TEST_L1_MISS=0
CONFIG_TEST_LLC_MISS=0

usage()
{
  echo "Usage:"
  echo " $0 -h          show help message"
  echo " $0 [-time] [-gprof] [-gperf] [-perf] [+SEC] [++THREADS] [PROGRAMS] [TESTS]"
  echo "        -time         run each program.time for time usage test"
  echo "        -gprof        run each program.gprof for graph profile test"
  echo "        -gperf        run each program.gperf for CPU profile (google perftools) test"
  echo "        -perf         run each program.perf for perf test"
  echo "        +SEC          run each test for SEC seconds, default 1, e.g. '+10'"
  echo "        ++THREADS     run each test in THREADS threads concurrently, default 1, e.g. '++4'"
  echo "        PROGRAMS      programs to run, seperated by space, e.g. 'prog1 prog2'"
  echo "        TESTS         only run TESTS in program, start with '.', e.g. '.ctor .out'"
  echo "  example:"
  echo "    $0 -time +10 ++4 example_pftest"
  exit 1
}

any_test=0
time_test=0
gprof_test=0
gperf_test=0
perf_test=0
seconds=
threads=
programs=
test_names=

for a in $* ; do
  case "$a" in
    "-time" )
      time_test=1
      any_test=1
      ;;
    "-gprof" )
      gprof_test=1
      any_test=1
      ;;
    "-gperf" )
      gperf_test=1
      any_test=1
      ;;
    "-perf" )
      perf_test=1
      any_test=1
      ;;
    "-h" )
      usage
      ;;
    * )
      if [ "${a:0:2}" = "++" ] ; then
        threads=$a
      elif [ "${a:0:1}" = "+" ] ; then
        seconds=$a
      elif [ "${a:0:1}" = "." ] ; then
        test_names+=" $a"
      else
        programs+=" $a"
      fi
      ;;
  esac
done

if [ $any_test -eq 0 ] ; then
  usage
fi

args="$seconds $threads $test_names"

# arguments test
if false ; then
  echo "time_test=$time_test"
  echo "gprof_test=$gprof_test"
  echo "gperf_test=$gperf_test"
  echo "perf_test=$perf_test"
  echo "seconds=$seconds"
  echo "threads=$threads"
  echo "programs=$programs"
  echo "test_names=$test_names"
  echo "args=$args"
  exit 0
fi


run_time()
{
  if [ -f ./$1 ] ; then
    echo "run $1:"
    ./$1 $args 
  else
    echo "'$1' not found"
    exit 1
  fi
}

run_time_all()
{
  for e in *.time ; do
    run_time $e
  done
}

run_gprof()
{
  if [ -f ./$1 ] ; then
    echo "run $1:"
    if ./$1 $args ; then
      gprof ./$1 gmon.out > $1.log
    fi
  else
    echo "'$1' not found"
    exit 1
  fi
}

run_gprof_all()
{
  for e in *.gprof ; do
    run_gprof $e
  done
}

if [ -e /usr/bin/google-pprof ] ; then
  GPROF="google-pprof"
else
  GPROF="pprof"
fi

run_gperf()
{
  if [ -f ./$1 ] ; then
    echo "run $1:"
    if ./$1 $args ; then
      $GPROF --text ./$1 1.pprof > $1.log
      FUN=`head -n 2 $1.log | tail -n 1 | awk '{print $6}'`
      if [ "$FUN" != "" ] ; then
        echo "$GPROF --list=$FUN ./$1 1.pprof >> $1.log"
        $GPROF --list=$FUN ./$1 1.pprof >> $1.log
      fi
    fi
  else
    echo "'$1' not found"
    exit 1
  fi
}

run_gperf_all()
{
  for e in *.gperf ; do
    run_gperf $e
  done
}

PERF=perf
#PERF_STAT_ARGS="-e task-clock -e context-switches -e CPU-migrations -e page-faults -e cycles -e instructions -e branches -e branch-misses -e L1-dcache-loads -e L1-dcache-load-misses -e cache-misses -e cache-references"
PERF_STAT_ARGS="-d"

run_perf()
{
  if [ -f ./$1 ] ; then
    echo "run $1:"
    rm -rf perf.data*
    echo "perf stat $PERF_STAT_ARGS ./$1 $args"
#    if ! perf stat $PERF_STAT_ARGS -o $1.log ./$1 $args ; then
    if ! $PERF stat $PERF_STAT_ARGS ./$1 $args 2&>$1.log ; then
      echo "'perf' not found"
      exit 1
    fi
    # cpu cycles
    echo "perf record -e cycles -a ./$1 $args"
    $PERF record -e cycles -a ./$1 $args && $PERF report -n --stdio | head -n 30 >> $1.log
    echo >>$1.log
    FUN=`grep -w Overhead -A 3 $1.log | head -n 4 | tail -n 1 | awk -F'\[.\] ' '{print $2}' | sed 's/^ *//;s/ *$//'`
    if [ "$FUN" != "" ] ; then
      echo "perf annotate -l -s '$FUN'"
      $PERF annotate -l -s "$FUN" >> $1.log
      echo >>$1.log
    fi
    mv perf.data perf.data.cycles
    # branch misses
    if [ $CONFIG_TEST_BRANCH_MISS -ne 0 ] ; then
      echo "perf record -e branch-misses -a ./$1 $args"
      $PERF record -e branch-misses -a ./$1 $args && $PERF report -n --stdio | head -n 30 >> $1.log
      echo >>$1.log
      mv perf.data perf.data.branch-misses
    fi
    # L1 dcache load misses
    if [ $CONFIG_TEST_L1_MISS -ne 0 ] ; then
      echo "perf record -e L1-dcache-load-misses -a ./$1 $args"
      $PERF record -e L1-dcache-load-misses -a ./$1 $args && $PERF report -n --stdio | head -n 30 >> $1.log
      echo >>$1.log
      mv perf.data perf.data.L1-dcache-load-misses
    fi
    # LLC load misses
    if [ $CONFIG_TEST_LLC_MISS -ne 0 ] ; then
      echo "perf record -e LLC-load-misses -a ./$1 $args"
      $PERF record -e LLC-load-misses -a ./$1 $args && $PERF report -n --stdio | head -n 30 >> $1.log
      echo >>$1.log
      mv perf.data perf.data.LLC-load-misses
    fi
  else
    echo "'$1' not found"
    exit 1
  fi
}

run_perf_all()
{
  for e in *.perf ; do
    run_perf $e
  done
}

test_p()
{
  if [ $time_test -eq 1 ] ; then
    run_time $1.time
  fi
  if [ $gprof_test -eq 1 ] ; then
    run_gprof $1.gprof
  fi
  if [ $gperf_test -eq 1 ] ; then
    run_gperf $1.gperf
  fi
  if [ $perf_test -eq 1 ] ; then
    run_perf $1.perf
  fi
}

test_p_all()
{
  if [ $time_test -eq 1 ] ; then
    run_time_all
  fi
  if [ $gprof_test -eq 1 ] ; then
    run_gprof_all
  fi
  if [ $gperf_test -eq 1 ] ; then
    run_gperf_all
  fi
  if [ $perf_test -eq 1 ] ; then
    run_perf_all
  fi
}

if [ "$programs" = "" ] ; then
  test_p_all
else
  for e in $programs ; do
    test_p $e
  done
fi
