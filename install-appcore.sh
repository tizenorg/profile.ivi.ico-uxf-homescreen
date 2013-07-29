#!/bin/sh

#library(libico-app-fw)
rm -fr /usr/lib/libico-app-fw-efl.*
cp ico-app-framework/.libs/libico-app-fw-efl.so.0.0.7 /usr/lib
ln -s /usr/lib/libico-app-fw-efl.so.0.0.7 /usr/lib/libico-app-fw-efl.so.0
ln -s /usr/lib/libico-app-fw-efl.so.0.0.7 /usr/lib/libico-app-fw-efl.so
chmod 0755 /usr/lib/libico-app-fw-efl.so.0.0.7
rm -fr /usr/lib/libico-app-fw.*
cp ico-app-framework/.libs/libico-app-fw.so.0.0.7 /usr/lib
ln -s /usr/lib/libico-app-fw.so.0.0.7 /usr/lib/libico-app-fw.so.0
ln -s /usr/lib/libico-app-fw.so.0.0.7 /usr/lib/libico-app-fw.so
chmod 0755 /usr/lib/libico-app-fw.so.0.0.7

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
rm -rf /usr/apps/$HOMESCREEN/var
rm -rf /usr/apps/$HOMESCREEN/res
mkdir -p /usr/apps/$HOMESCREEN/res/images
mkdir -p /usr/apps/$HOMESCREEN/res/edj
mkdir -p /usr/apps/$HOMESCREEN/res/config
cp -rf res/config /usr/apps/$HOMESCREEN/res
cp -rf data/apps/$HOMESCREEN /usr/apps/
mkdir -p /usr/apps/$HOMESCREEN/bin/
cp src/HomeScreen /usr/apps/$HOMESCREEN/bin/
cp src/home_screen_bg.edj /usr/apps/$HOMESCREEN/res/edj
cp src/home_screen_touch.edj /usr/apps/$HOMESCREEN/res/edj
chmod 666 /usr/apps/$HOMESCREEN/res/edj/home_screen_*
cp res/images/api_all_off.png /usr/apps/$HOMESCREEN/res/images
cp res/images/api_all_on.png /usr/apps/$HOMESCREEN/res/images
chmod 666 /usr/apps/$HOMESCREEN/res/images/api_all_*
cp data/share/applications/$HOMESCREEN.desktop /usr/share/applications/
cp data/share/packages/$HOMESCREEN.xml /usr/share/packages/
cp res/apps/$HOMESCREEN/* /usr/apps/$HOMESCREEN/res/config

#StatusBar
PKGNAME=org.tizen.ico.statusbar
rm -rf /usr/apps/$PKGNAME/res
mkdir -p /usr/apps/$PKGNAME/res/images
mkdir -p /usr/apps/$PKGNAME/res/edj
mkdir -p /usr/apps/$PKGNAME/res/config
cp -rf data/apps/$PKGNAME /usr/apps/
mkdir -p /usr/apps/$PKGNAME/bin/
cp src/StatusBar /usr/apps/$PKGNAME/bin/
cp res/images/time*.png /usr/apps/$PKGNAME/res/images
cp res/images/applist_*.png /usr/apps/$PKGNAME/res/images
cp res/images/home*.png /usr/apps/$PKGNAME/res/images
chmod 666 /usr/apps/$PKGNAME/res/images/time*.png
cp data/share/applications/$PKGNAME.desktop /usr/share/applications/
cp data/share/packages/$PKGNAME.xml /usr/share/packages/
cp res/apps/$PKGNAME/* /usr/apps/$PKGNAME/res/config

#OnScreen
PKGNAME=org.tizen.ico.onscreen
rm -rf /usr/apps/$PKGNAME/res
mkdir -p /usr/apps/$PKGNAME/res/images
mkdir -p /usr/apps/$PKGNAME/res/edj
mkdir -p /usr/apps/$PKGNAME/res/config
cp -rf data/apps/$PKGNAME /usr/apps/
mkdir -p /usr/apps/$PKGNAME/bin/
cp src/OnScreen /usr/apps/$PKGNAME/bin/
cp src/appli_list.edj /usr/apps/$PKGNAME/res/edj
cp src/appli_kill.edj /usr/apps/$PKGNAME/res/edj
chmod 666 /usr/apps/$PKGNAME/res/edj/*.edj
chmod 666 /usr/apps/$PKGNAME/res/images/*.png > /dev/null 2>&1
cp data/share/applications/$PKGNAME.desktop /usr/share/applications/
cp data/share/packages/$PKGNAME.xml /usr/share/packages/
cp res/apps/$PKGNAME/* /usr/apps/$PKGNAME/res/config

#reload AppCore database
/usr/bin/pkg_initdb
/usr/bin/ail_initdb

