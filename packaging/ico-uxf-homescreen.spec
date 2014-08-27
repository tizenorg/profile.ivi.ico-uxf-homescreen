Name:       ico-uxf-homescreen
Summary:    Sample homescreen and system controller
Version:    0.9.22
Release:    0
Group:      Automotive / ICO Homescreen
License:    Apache-2.0
URL:        ""
Source0:    %{name}-%{version}.tar.bz2

BuildRequires: pkgconfig(wayland-client) >= 1.4
BuildRequires: ico-uxf-weston-plugin-devel >= 0.9.21
BuildRequires: pkgconfig(egl)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(ecore-wayland)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(edje)
BuildRequires: pkgconfig(dbus-1)
BuildRequires: pkgconfig(json-glib-1.0)
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(ail)
BuildRequires: pkgconfig(bundle)
BuildRequires: pkgconfig(pkgmgr)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(libwebsockets)
BuildRequires: pkgconfig(murphy-common)
BuildRequires: pkgconfig(murphy-domain-controller)
BuildRequires: pkgconfig(murphy-ecore)
BuildRequires: pkgconfig(murphy-resource)
BuildRequires: pkgconfig(notification)
BuildRequires: pkgconfig(appsvc)
BuildRequires: boost-devel
BuildRequires: mesa-devel
BuildRequires: weston-ivi-shell
BuildRequires: weston-ivi-shell-devel
BuildRequires: genivi-shell
BuildRequires: genivi-shell-devel
BuildRequires: ico-uxf-utilities-devel >= 0.9.07
Requires: weston >= 1.4
Requires: weston-ivi-shell
Requires: genivi-shell
Requires: ico-uxf-weston-plugin >= 0.9.21
Requires: ico-uxf-utilities >= 0.9.07
Requires: system-controller

%description
Sample homescreen application.


%package system-controller
Summary: System controller for ICO HomeScreen.
Group:   Automotive / ICO Homescreen
Requires: %{name} = %{version}-%{release}
Requires: weston >= 1.4
Requires: weston-ivi-shell
Requires: genivi-shell
Requires: ico-uxf-weston-plugin >= 0.9.21
Requires: ico-uxf-utilities >= 0.9.07
Provides: system-controller

%description system-controller
A service to provide the low-level functionality that ICO HomeScreen
requires.


%package system-controller-devel
Summary:  Development files for %{name}
Group:    Automotive / ICO Homescreen
Requires: %{name} = %{version}-%{release}
Requires: capi-base-common-devel
Requires: pkgconfig(eina)
Requires: pkgconfig(evas)
Requires: pkgconfig(eina)
Requires: pkgconfig(edje)
Requires: pkgconfig(ecore-wayland)
Requires: pkgconfig(glib-2.0)
Requires: pkgconfig(appsvc)

%description system-controller-devel
Development files for application that communicate homescreen.

%prep
%setup -q -n %{name}-%{version}

mkdir -p weston
cp -av /usr/share/genivi-shell/protocol/*.xml weston/

ls weston/*.xml | while read protocol ; do

    /usr/bin/wayland-scanner client-header \
        < "${protocol}" > "${protocol/.xml/}-client-protocol.h"

    /usr/bin/wayland-scanner code \
        < "${protocol}" > "${protocol/.xml/.c}"

done

ls weston/


%build
%autogen
make %{?_smp_mflags}

%install
rm -rf %{buildroot}

%make_install

cp tool/ico_clear_screen %{buildroot}%{_bindir}
cp tool/ico_change_loginuser %{buildroot}%{_bindir}
cp tool/notification/ico_del_notification %{buildroot}%{_bindir}
cp tool/notification/ico_dump_notification %{buildroot}%{_bindir}
cp tool/notification/ico_send_notification %{buildroot}%{_bindir}
cp tool/notification/ico_send_notification2 %{buildroot}%{_bindir}

%post
/sbin/ldconfig

# Update the app database.
%{_bindir}/pkginfo --imd /usr/share/packages/org.tizen.ico.homescreen.xml
%{_bindir}/pkginfo --imd /usr/share/packages/org.tizen.ico.statusbar.xml
%{_bindir}/pkginfo --imd /usr/share/packages/org.tizen.ico.system-controller.xml
%{_bindir}/pkginfo --imd /usr/share/packages/org.tizen.ico.onscreen.xml

%preun
# Update the app database.
%{_bindir}/pkginfo --rmd /usr/share/packages/org.tizen.ico.homescreen.xml
%{_bindir}/pkginfo --rmd /usr/share/packages/org.tizen.ico.statusbar.xml
%{_bindir}/pkginfo --rmd /usr/share/packages/org.tizen.ico.system-controller.xml
%{_bindir}/pkginfo --rmd /usr/share/packages/org.tizen.ico.onscreen.xml

%postun
/sbin/ldconfig
rm -f /usr/share/applications/org.tizen.ico.homescreen.desktop
rm -f /usr/share/applications/org.tizen.ico.statusbar.desktop
rm -f /usr/share/applications/org.tizen.ico.onscreen.desktop
rm -f /usr/share/applications/org.tizen.ico.system-controller.desktop

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.ico.homescreen
/usr/apps/org.tizen.ico.statusbar
/usr/apps/org.tizen.ico.onscreen
/usr/share/packages/org.tizen.ico.homescreen.xml
/usr/share/packages/org.tizen.ico.statusbar.xml
/usr/share/packages/org.tizen.ico.onscreen.xml
%{_libdir}/libico-appfw.*
%{_libdir}/libico-state-machine.*
%{_bindir}/ico_clear_screen
%{_bindir}/ico_change_loginuser
%{_bindir}/ico_del_notification
%{_bindir}/ico_dump_notification
%{_bindir}/ico_send_notification
%{_bindir}/ico_send_notification2
%attr(644,app,app) /home/app/ico/defaultApps.info
%attr(755,app,app) /home/app/ico

%files system-controller
%manifest %{name}.manifest
%defattr(-,root,root,-)
/usr/share/packages/org.tizen.ico.system-controller.xml
/usr/lib/systemd/user/ico-uxf-wait-launchpad-ready.path
/usr/lib/systemd/user/weston.target.wants/ico-uxf-wait-launchpad-ready.path
/usr/lib/systemd/user/ico-system-controller.service
/usr/apps/org.tizen.ico.system-controller
%attr(644,app,app) /home/app/ico/defaultApps.info
%attr(755,app,app) /home/app/ico

%files system-controller-devel
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_includedir}/ico-appfw/ico_syc_application.h
%{_includedir}/ico-appfw/ico_syc_appresctl.h
%{_includedir}/ico-appfw/ico_syc_common.h
%{_includedir}/ico-appfw/ico_syc_error.h
%{_includedir}/ico-appfw/ico_syc_inputctl.h
%{_includedir}/ico-appfw/ico_syc_private.h
%{_includedir}/ico-appfw/ico_syc_privilege.h
%{_includedir}/ico-appfw/ico_syc_type.h
%{_includedir}/ico-appfw/ico_syc_userctl.h
%{_includedir}/ico-appfw/ico_syc_winctl.h
%{_includedir}/ico-state-machine/CicoBlockParser.h
%{_includedir}/ico-state-machine/CicoEvent.h
%{_includedir}/ico-state-machine/CicoEventInfo.h
%{_includedir}/ico-state-machine/CicoFinalState.h
%{_includedir}/ico-state-machine/CicoHistoryState.h
%{_includedir}/ico-state-machine/CicoState.h
%{_includedir}/ico-state-machine/CicoStateAction.h
%{_includedir}/ico-state-machine/CicoStateCore.h
%{_includedir}/ico-state-machine/CicoStateMachine.h
%{_includedir}/ico-state-machine/CicoStateMachineCreator.h

