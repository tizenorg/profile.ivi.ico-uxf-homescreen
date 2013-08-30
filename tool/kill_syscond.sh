#!/bin/sh
killall ico_ictl-touch_egalax 2> /dev/null
killall weston 2> /dev/null
killall IcoSysconDaemon 2> /dev/null

sleep 2

killall -9 DummyHomeScreen 2> /dev/null
killall -9 HomeScreen 2> /dev/null
killall -9 IcoSysconDaemon 2> /dev/null
killall -9 weston 2> /dev/null
killall -9 ico_ictl-touch_egalax 2> /dev/null

killall -9 OnScreen 2> /dev/null
killall -9 StatusBar 2> /dev/null
killall -9 browser 2> /dev/null
killall -9 gallery 2> /dev/null
killall -9 music-player 2> /dev/null
killall -9 setting 2> /dev/null
killall -9 calendar 2> /dev/null
killall -9 dialer 2> /dev/null
killall -9 calculator 2> /dev/null
killall -9 memo 2> /dev/null
killall -9 smartsearch 2> /dev/null
killall -9 GV3ySIINq7.GhostCluster 2> /dev/null
killall -9 t8j6HTRpuz.MediaPlayer 2> /dev/null
killall -9 WebProcess 2> /dev/null
killall -9 WebProcess 2> /dev/null
killall -9 ico-app-login 2> /dev/null
kill -9 `ps ax | grep wrt_launchpad_daemon_candidate | grep -v grep | cut -c1-6` 2> /dev/null
killall -9 WebProcess 2> /dev/null
kill -9 `ps ax | grep wrt_launchpad_daemon_candidate | grep -v grep | cut -c1-6` 2> /dev/null
killall -9 WebProcess 2> /dev/null
kill -9 `ps ax | grep wrt_launchpad_daemon_candidate | grep -v grep | cut -c1-6` 2> /dev/null
