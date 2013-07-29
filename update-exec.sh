#!/bin/sh

#library(libico-app-fw)
rm -fr /usr/lib/libico-app-fw-efl.*
cp ico-app-framework/.libs/libico-app-fw-efl.so.0.0.5 /usr/lib
ln -s /usr/lib/libico-app-fw-efl.so.0.0.5 /usr/lib/libico-app-fw-efl.so.0
ln -s /usr/lib/libico-app-fw-efl.so.0.0.5 /usr/lib/libico-app-fw-efl.so
chmod 0755 /usr/lib/libico-app-fw-efl.so.0.0.5
rm -fr /usr/lib/libico-app-fw.*
cp ico-app-framework/.libs/libico-app-fw.so.0.0.5 /usr/lib
ln -s /usr/lib/libico-app-fw.so.0.0.5 /usr/lib/libico-app-fw.so.0
ln -s /usr/lib/libico-app-fw.so.0.0.5 /usr/lib/libico-app-fw.so
chmod 0755 /usr/lib/libico-app-fw.so.0.0.5

#test tools
rm -fr /usr/bin/ico_send_hscommand
rm -fr /usr/bin/ico_send_inputevent
rm -fr /usr/bin/ico_set_vehicleinfo
cp test/ico_send_hscommand /usr/bin
cp test/ico_send_inputevent /usr/bin
cp test/ico_set_vehicleinfo /usr/bin
chmod 0755 /usr/bin/ico_send_hscommand /usr/bin/ico_send_inputevent /usr/bin/ico_set_vehicleinfo

#HomeScreen
HOMESCREEN=org.tizen.ico.homescreen
cp src/HomeScreen /usr/apps/$HOMESCREEN/bin/

#StatusBar
PKGNAME=org.tizen.ico.statusbar
cp src/StatusBar /usr/apps/$PKGNAME/bin/

#OnScreen
PKGNAME=org.tizen.ico.onscreen
cp src/OnScreen /usr/apps/$PKGNAME/bin/

