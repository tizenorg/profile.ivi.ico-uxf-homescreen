#!/bin/sh

rm -rf /opt/apps/org.tizen.ico.dummy*
rm -rf /opt/share/applications/org.tizen.ico.dummy*

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

#HomeScreen
HOMESCREEN=org.tizen.ico.homescreen
rm -rf /opt/apps/$HOMESCREEN/var
rm -rf /opt/apps/$HOMESCREEN/res
mkdir -p /opt/apps/$HOMESCREEN/res/images
mkdir -p /opt/apps/$HOMESCREEN/res/edj
mkdir -p /opt/apps/$HOMESCREEN/res/config
cp -rf res/config /opt/apps/$HOMESCREEN/res
cp -rf data/apps/$HOMESCREEN /opt/apps/
mkdir -p /opt/apps/$HOMESCREEN/bin/
cp src/HomeScreen /opt/apps/$HOMESCREEN/bin/
cp src/home_screen_bg.edj /opt/apps/$HOMESCREEN/res/edj
cp src/home_screen_touch.edj /opt/apps/$HOMESCREEN/res/edj
chmod 666 /opt/apps/$HOMESCREEN/res/edj/home_screen_*
cp res/images/api_all_off.png /opt/apps/$HOMESCREEN/res/images
cp res/images/api_all_on.png /opt/apps/$HOMESCREEN/res/images
chmod 666 /opt/apps/$HOMESCREEN/res/images/api_all_*
cp data/share/applications/$HOMESCREEN.desktop /opt/share/applications/
cp res/apps/$HOMESCREEN/* /opt/apps/$HOMESCREEN/res/config

#StatusBar
PKGNAME=org.tizen.ico.statusbar
rm -rf /opt/apps/$PKGNAME/res
mkdir -p /opt/apps/$PKGNAME/res/images
mkdir -p /opt/apps/$PKGNAME/res/edj
mkdir -p /opt/apps/$PKGNAME/res/config
cp -rf data/apps/$PKGNAME /opt/apps/
mkdir -p /opt/apps/$PKGNAME/bin/
cp src/StatusBar /opt/apps/$PKGNAME/bin/
cp res/images/time*.png /opt/apps/$PKGNAME/res/images
chmod 666 /opt/apps/$PKGNAME/res/images/time*.png
cp data/share/applications/$PKGNAME.desktop /opt/share/applications/
cp res/apps/$PKGNAME/* /opt/apps/$PKGNAME/res/config

#OnScreen
PKGNAME=org.tizen.ico.onscreen
rm -rf /opt/apps/$PKGNAME/res
mkdir -p /opt/apps/$PKGNAME/res/images
mkdir -p /opt/apps/$PKGNAME/res/edj
mkdir -p /opt/apps/$PKGNAME/res/config
cp -rf data/apps/$PKGNAME /opt/apps/
mkdir -p /opt/apps/$PKGNAME/bin/
cp src/OnScreen /opt/apps/$PKGNAME/bin/
cp src/appli_list.edj /opt/apps/$PKGNAME/res/edj
cp src/appli_kill.edj /opt/apps/$PKGNAME/res/edj
chmod 666 /opt/apps/$PKGNAME/res/edj/*.edj
chmod 666 /opt/apps/$PKGNAME/res/images/*.png
cp data/share/applications/$PKGNAME.desktop /opt/share/applications/
cp res/apps/$PKGNAME/* /opt/apps/$PKGNAME/res/config

#delete dummy apps
rm -rf /opt/apps/org.tizen.ico.sample.*
rm -f  /opt/share/applications/org.tizen.ico.sample.*

#reload AppCore database
cd /opt/dbspace
rm -rf .app_info*
/usr/bin/ail_initdb

