#!/bin/bash

DIR=`pwd`
DIR=${DIR##*\/}

server=10.134.9.104     #tlinux
#server=172.27.207.118  #suse64
#server=10.169.12.242
port=8081

# include & exclude file pattern
INC_FILE="*.h *.hh *.c *.cc *.proto *.license *.srcc *.sh *.txt *.py *.conf config/*.conf c2_share/**/* *.sql Makefile* *.mk *.mak *.list ENVIRONMENT *.gz ack"	# include file pattern
EXC_FILE="doc/ .svn/ .cvs/ CVS/ *.pb.h *.pb.cc"	# exclude file pattern

INC_FILE=$(echo "${INC_FILE}" | sed -e 's/\*/===/g')
FILTER=`
for exc in $EXC_FILE ; do
	echo " -f '- $exc'"
done 
for inc in $INC_FILE ; do
	echo " -f '+ $inc'"
done
echo " -f '+ */' -f '- *'"`
FILTER=$(echo "${FILTER}" | sed -e 's/===/\*/g')

RSYNC_CMD="rsync -vzcrlptD $FILTER"

[ -r ~/t64.sec ] && . ~/t64.sec
[ -r t64.sec ] && . t64.sec

CMD="${RSYNC_CMD} --port=${port} ./ root@${server}::$(whoami)/${DIR}"

echo $CMD | /bin/bash

