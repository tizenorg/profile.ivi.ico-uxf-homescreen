Name:       ico-uxf-homescreen
Summary:    Sample homescreen and system controller
Version:    0.9.22
Release:    0
Group:      Automotive/ICO Homescreen
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.bz2
Source1001: %{name}.manifest

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
BuildRequires: pkgconfig(libtzplatform-config)
BuildRequires: fdupes
Requires: weston >= 1.4
Requires: weston-ivi-shell
Requires: genivi-shell
Requires: ico-uxf-weston-plugin >= 0.9.21
Requires: ico-uxf-utilities >= 0.9.07
Requires: system-controller

Requires(post): pkgmgr
Requires(post): ail

%description
This package provides the sample homescreen application.


%package system-controller-devel
Summary:  Development files for %{name}
Group:    Automotive/ICO Homescreen
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
cp %{SOURCE1001} .

mkdir -p weston
cp -av %{TZ_SYS_SHARE}/genivi-shell/protocol/*.xml weston/

ls weston/*.xml | while read protocol ; do

    %{_bindir}/wayland-scanner client-header \
        < "${protocol}" > "${protocol/.xml/}-client-protocol.h"

    %{_bindir}/wayland-scanner code \
        < "${protocol}" > "${protocol/.xml/.c}"

done

%build
%autogen
%__make %{?_smp_mflags}

%install
rm -rf %{buildroot}

%make_install

cp tool/ico_clear_screen %{buildroot}%{_bindir}
cp tool/ico_change_loginuser %{buildroot}%{_bindir}
cp tool/notification/ico_del_notification %{buildroot}%{_bindir}
cp tool/notification/ico_dump_notification %{buildroot}%{_bindir}
cp tool/notification/ico_send_notification %{buildroot}%{_bindir}
cp tool/notification/ico_send_notification2 %{buildroot}%{_bindir}

%fdupes %{buildroot}

%post
/sbin/ldconfig
# Update the app database.
pkg_initdb
ail_initdb

%postun
/sbin/ldconfig
rm -f %{TZ_SYS_RW_DESKTOP_APP}/org.tizen.ico.homescreen.desktop
rm -f %{TZ_SYS_RW_DESKTOP_APP}/org.tizen.ico.statusbar.desktop
rm -f %{TZ_SYS_RW_DESKTOP_APP}/org.tizen.ico.onscreen.desktop
pkg_initdb
ail_initdb

%files
%defattr(-,root,root,-)
%manifest %{name}.manifest
%{TZ_SYS_RW_APP}/org.tizen.ico.homescreen
%{TZ_SYS_RW_APP}/org.tizen.ico.statusbar
%{TZ_SYS_RW_APP}/org.tizen.ico.onscreen
%{TZ_SYS_RW_PACKAGES}/org.tizen.ico.homescreen.xml
%{TZ_SYS_RW_PACKAGES}/org.tizen.ico.statusbar.xml
%{TZ_SYS_RW_PACKAGES}/org.tizen.ico.onscreen.xml
%{_libdir}/libico-appfw.*
%{_bindir}/ico_clear_screen
%{_bindir}/ico_change_loginuser
%{_bindir}/ico_del_notification
%{_bindir}/ico_dump_notification
%{_bindir}/ico_send_notification
%{_bindir}/ico_send_notification2
%{_datadir}/icons/default/small/org.tizen.ico.homescreen.png
%{_datadir}/icons/default/small/org.tizen.ico.statusbar.png
%{_datadir}/icons/default/small/org.tizen.ico.onscreen.png
# retain the system control configuration inside homescreen at least for now 
/usr/apps/org.tizen.ico.system-controller/res/config

%files system-controller-devel
%defattr(-,root,root,-)
%manifest %{name}.manifest
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

