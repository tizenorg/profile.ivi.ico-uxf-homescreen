#!/bin/sh

# 1 Delete log file
rm -fr /var/log/weston.log

# 2 Weston/Wayland Envionment
export XDG_RUNTIME_DIR=/tmp/run-root
export QT_QPA_PLATFORM=wayland
export ELM_ENGINE=wayland_egl
export ECORE_EVAS_ENGINE=wayland_egl
#export ELM_ENGINE=wayland_shm
#export ECORE_EVAS_ENGINE=wayland_shm
export EVAS_FONT_DPI=72
export ECORE_IMF_MODULE=isf
export ELM_MODULES="ctxpopup_copypasteUI>entry/api:datetime_input_ctxpopup>datetime/api"
export ELM_SCALE="0.7"
export ELM_PROFILE=mobile

# 3 Start Device Input Controller for eGalax TouchPanel
/usr/bin/ico_ictl-touch_egalax -t
sleep 0.3

# 4 Start Weston
ORG_UMASK=`umask`
umask 000
/usr/bin/weston --backend=drm-backend.so --idle-time=0 --log=/var/log/weston.log &
sleep 0.5
if [ -f $XDG_RUNTIME_DIR/wayland-0 ] ; then
	chmod 0777 $XDG_RUNTIME_DIR/wayland-0
fi
umask $ORG_UMASK

# 5 start app core daemon
sleep 1
/usr/bin/launchpad_preloading_preinitializing_daemon &

# 6 start homescreen
rm -fr /tmp/log
mkdir /tmp/log/
chmod -R 0777 /tmp/log
/usr/bin/launch_app org.tizen.ico.homescreen &

# 7 start test shell
sleep 5
./homescreen-test.sh
