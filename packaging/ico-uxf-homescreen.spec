Name:       ico-uxf-homescreen
Summary:    Sample homescreen and system controller
Version:    0.9.12
Release:    1.2
Group:		Graphics & UI Framework/Automotive UI
License:    Apache-2.0
URL:        ""
Source0:    %{name}-%{version}.tar.bz2

BuildRequires: pkgconfig(wayland-client) >= 1.2
BuildRequires: ico-uxf-weston-plugin-devel >= 0.9.09
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(ecore-wayland)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(edje)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(edbus)
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
BuildRequires: boost-devel
BuildRequires: mesa-devel
BuildRequires: ico-uxf-utilities-devel >= 0.9.06
Requires: weston >= 1.2
Requires: ico-uxf-weston-plugin >= 0.9.09
Requires: ico-uxf-utilities >= 0.9.06

%description
Sample homescreen application and system controller daemon

%package system-controller-devel
Summary:  Development files for %{name}
Group:    Graphics & UI Framework/Development
Requires: %{name} = %{version}-%{release}
Requires: capi-base-common-devel
Requires: pkgconfig(eina)
Requires: pkgconfig(evas)
Requires: pkgconfig(eina)
Requires: pkgconfig(edje)
Requires: pkgconfig(elementary)
Requires: pkgconfig(ecore-wayland)
Requires: pkgconfig(ecore-x)
Requires: pkgconfig(glib-2.0)

%description system-controller-devel
Development files for application that communicate homescreen.

%prep
%setup -q -n %{name}-%{version}

%build
%autogen
make %{?_smp_mflags}

%install
rm -rf %{buildroot}

%make_install

cp tool/ico_clear_screen %{buildroot}%{_bindir}
cp tool/ico_change_loginuser %{buildroot}%{_bindir}

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
/usr/apps/org.tizen.ico.system-controller
/usr/share/packages/org.tizen.ico.system-controller.xml
/usr/lib/systemd/user/ico-system-controller.service
/usr/lib/systemd/user/ico-uxf-wait-launchpad-ready.path
/usr/lib/systemd/user/weston.target.wants/ico-uxf-wait-launchpad-ready.path
%{_bindir}/ico_clear_screen
%{_bindir}/ico_change_loginuser
%attr(644,app,app) /home/app/ico/defaultApps.info
%attr(755,app,app) /home/app/ico
/home/app/ico/defaultApps.info

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