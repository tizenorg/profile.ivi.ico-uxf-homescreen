#!/bin/sh

# 1. Delete log file
/bin/mkdir /var/log/ico > /dev/null 2>&1
/bin/mkdir /tmp/ico > /dev/null 2>&1
/bin/chmod -R 0777 /var/log/ico > /dev/null 2>&1
/bin/chmod -R 0777 /tmp/ico > /dev/null 2>&1
/bin/rm -f /var/log/ico/* /var/log/weston.log > /dev/null 2>&1
/bin/rm -f /tmp/ico/* > /dev/null 2>&1
/bin/rm -f /opt/share/crash/core.* > /dev/null 2>&1
/usr/bin/pkg_initdb > /dev/null 2>&1
/usr/bin/ail_initdb > /dev/null 2>&1

# 2. Start Device Input Controller for eGalax TouchPanel
#/usr/bin/ico_ictl-touch_egalax -t
/usr/bin/ico_ictl-touch_egalax
sleep 0.3

# 3. Start Weston
/bin/ps ax | /bin/grep weston | /bin/grep -v grep > /dev/null
if [ "$?" = "1" ] ; then
	/usr/bin/weston-launch --user app -- -i0 --log=/var/log/ico/weston.log &
	sync;sync
fi

/usr/bin/launch_app org.tizen.ico.system-controller &
