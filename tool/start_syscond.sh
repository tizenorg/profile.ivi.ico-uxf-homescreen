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

# 4 start some daemons
## if pulseaudio dose not start ... kick pulseaudio
/bin/ps ax | /bin/grep pulseaudio | /bin/grep -v grep > /dev/null
if [ "$?" = "1" ] ; then
	/usr/bin/pulseaudio --log-level=4 --log-target=file:/var/log/ico/pulse.log --system -D
	sleep 0.5
fi

# 3. Start Weston
/bin/ps ax | /bin/grep weston | /bin/grep -v grep > /dev/null
if [ "$?" = "1" ] ; then
	#/usr/bin/weston-launch --user app -- -i0 --backend=fbdev-backend.so --log=/var/log/ico/weston.log &
	/usr/bin/weston-launch --user app -- -i0 --log=/var/log/ico/weston.log &
	sync;sync
fi

/usr/bin/launch_app org.tizen.ico.system-controller &
