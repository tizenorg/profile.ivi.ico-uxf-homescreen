#!/bin/sh

# 1. Delete log file
/bin/mkdir /var/log/ico > /dev/null 2>&1
/bin/chmod -R 0777 /var/log/ico > /dev/null 2>&1
/bin/chmod -R 0777 /tmp/ico > /dev/null 2>&1
/bin/rm -f /var/log/ico/* /var/log/weston.log > /dev/null 2>&1
/bin/rm -f /opt/share/crash/core.* > /dev/null 2>&1
/bin/rm -f /opt/share/crash/info/* > /dev/null 2>&1

# 2. Setup Environment Valiables
export XDG_RUNTIME_DIR=/run/user/5000
/bin/mkdir /run/user/5000 > /dev/null 2>&1
/usr/bin/chown 5000 /run/user/5000
/usr/bin/chgrp 5000 /run/user/5000
/bin/rm -f /run/user/5000/wayland*

# 3. Start Weston
echo "export XDG_RUNTIME_DIR=/run/user/5000" > /etc/profile.d/start_syscond.sh
/usr/bin/weston-launch --user app -- -i0 $WESTON_BACKEND --log=/var/log/ico/weston.log &
/usr/bin/sync;/usr/bin/sync
/usr/bin/sleep 0.2
/usr/bin/ico_dic-gtforce -d &

# 4. Start MeterApp
/usr/bin/MeterApp > /dev/null 2>&1 &
/usr/bin/sleep 0.3

# 5. Start System Controller
/usr/bin/launch_app org.tizen.ico.system-controller &

# 6. End of shell
/usr/bin/sleep 2
/bin/rm -f /etc/profile.d/start_syscond.sh

