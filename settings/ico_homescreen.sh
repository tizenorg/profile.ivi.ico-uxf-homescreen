#!/bin/sh
/usr/bin/echo "launch kick HomeScreen sleeping"
sleep 20
/usr/bin/echo "launch kick HomeScreen org.tizen.ico.homescreen"
/usr/bin/launch_app org.tizen.ico.homescreen &
/usr/bin/echo "launch end  HomeScreen org.tizen.ico.homescreen"

