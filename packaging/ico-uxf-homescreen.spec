Name:       ico-uxf-homescreen
Summary:    Sample homescreen and system controller
Version:    0.9.31
Release:    1.1
Group:      Automotive / ICO Homescreen
License:    Apache-2.0
URL:        ""
Source0:    %{name}-%{version}.tar.bz2

BuildRequires: pkgconfig(wayland-client) >= 1.5
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
BuildRequires: weston-ivi-shell-devel >= 0.1.7
BuildRequires: genivi-shell-devel >= 0.2.3
BuildRequires: ico-uxf-utilities-devel >= 0.9.07
Requires: weston >= 1.5
Requires: weston-ivi-shell >= 0.1.7
Requires: genivi-shell >= 0.2.3
Requires: ico-uxf-utilities >= 0.9.07
Requires: system-controller
Conflicts: weston-ivi-config
Conflicts: weston-ivi-shell-config

%description
Sample homescreen application.


%package system-controller
Summary: System controller for ICO HomeScreen
Group:   Automotive / ICO Homescreen
Requires: %{name} = %{version}-%{release}
Requires: weston >= 1.5
Requires: weston-ivi-shell >= 0.1.7
Requires: genivi-shell >= 0.2.3
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

cp tool/ico_change_loginuser %{buildroot}%{_bindir}

# configurations
%define weston_conf %{_sysconfdir}/xdg/weston
%define ecore_setting %{_sysconfdir}/profile.d
mkdir -p %{buildroot}%{weston_conf} > /dev/null 2>&1
mkdir -p %{buildroot}%{ecore_setting} > /dev/null 2>&1
install -m 0644 settings/weston.ini %{buildroot}%{weston_conf}
install -m 0644 settings/ecore.sh %{buildroot}%{ecore_setting}

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
%{_bindir}/ico_change_loginuser
%{weston_conf}/weston.ini
%{ecore_setting}/ecore.sh

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

