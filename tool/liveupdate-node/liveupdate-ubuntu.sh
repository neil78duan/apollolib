#! /bin/sh
#
#

PATH=/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin/
WORKING=/home/liveupdate/liveupdate-node

CONFIG_FILE=$WORKING/configuration.json

DAEMON=$WORKING/index.js
PIDFILE=/var/run/liveupdate.pid
PROG="liveupdate"

OPTS="--path $WORKING --pidfile $PIDFILE --config $CONFIG_FILE "


test -x $DAEMON || exit 0

. /lib/lsb/init-functions

case "$1" in
  start)
	log_daemon_msg "Starting deferred execution scheduler" "$PROG"
	start_daemon $DAEMON  $OPTS & > /dev/null
	log_end_msg $?
    ;;
  stop)
	log_daemon_msg "Stopping deferred execution scheduler" "$PROG"
	killproc -p $PIDFILE $DAEMON
	log_end_msg $?
	rm -rf $PIDFILE
    ;;
  force-reload|restart)
    $0 stop
    $0 start
    ;;
  status)
    status_of_proc -p $PIDFILE $DAEMON $PROG && exit 0 || exit $?
    ;;
  *)
    echo "Usage: /etc/init.d/liveupdate {start|stop|restart|force-reload|status}"
    exit 1
    ;;
esac

exit 0
