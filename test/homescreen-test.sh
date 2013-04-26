#!/bin/sh

## log file
export HOMESCREEN_LOG=/tmp/log/org.tizen.ico.homescreen.log
export ONSCREEN_LOG=/tmp/log/org.tizen.ico.onscreen.log

## make dat
./homescreen-test-makedat.sh

## GUI test
./ico_send_inputevent -m < testdata/gui-test.dat

## CUI test
#Hide tile screen
./ico_send_hscommand json/hide-tile.json
sleep 3
#Show tile screen
./ico_send_hscommand json/show-tile.json
sleep 3
#Show application
./ico_send_hscommand json/hide-tile.json
sleep 3
./ico_send_hscommand json/show-app1.json
sleep 3
#Move and resize application
./ico_send_hscommand json/geom-app1-1.json
sleep 3
./ico_send_hscommand json/geom-app1-2.json
sleep 3
./ico_send_hscommand json/geom-app1-3.json
sleep 3
#Terminate application
./ico_send_hscommand json/term-app1.json
sleep 3
./ico_send_hscommand json/show-tile.json
sleep 3
#Terminate application
./ico_send_hscommand json/hide-tile.json
sleep 3
./ico_send_hscommand json/exec-app1.json
sleep 3
#Application on tile screen
./ico_send_hscommand json/show-tile.json
sleep 3

## Check Error
FOUND_ERR=0
/bin/grep -i "err" $HOMESCREEN_LOG
if [ "$?" != "1" ] ; then
    FOUND_ERR=1
fi
/bin/grep -i "err" $ONSCREEN_LOG
if [ "$?" != "1" ] ; then
    FOUND_ERR=1
fi
/bin/grep -i "fail" $HOMESCREEN_LOG
if [ "$?" != "1" ] ; then
    FOUND_ERR=1
fi
/bin/grep -i "fail" $ONSCREEN_LOG
if [ "$?" != "1" ] ; then
    FOUND_ERR=1
fi

## delete dat file
rm -f testdata/gui-test.dat

if [ $FOUND_ERR = 0 ] ; then
	echo "org.tizen.ico.homescreen Test: OK"
else
    echo "org.tizen.ico.homescreen Test: ERROR"
fi

