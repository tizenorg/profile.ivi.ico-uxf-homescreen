Name:       ico-uxf-homescreen
Summary:    sample homescreen
Version:    0.7.01
Release:    1.1
Group:		TO_BE/FILLED_IN
License:    Apache License, Version 2.0
URL:        ""
Source0:    %{name}-%{version}.tar.bz2

BuildRequires: pkgconfig(wayland-client) >= 1.2
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: ico-uxf-weston-plugin-devel >= 0.7
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(edje)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(ecore-wayland)
BuildRequires: pkgconfig(ecore-x)
BuildRequires: pkgconfig(dbus-1)
BuildRequires: pkgconfig(json-glib-1.0)
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(bundle)
BuildRequires: pkgconfig(ail)
BuildRequires: pkgconfig(pkgmgr)
BuildRequires: libwebsockets-devel
BuildRequires: capi-base-common-devel
BuildRequires: edje-tools
BuildRequires: ico-uxf-utilities-devel
Requires: weston >= 1.2
Requires: ico-uxf-weston-plugin >= 0.7
Requires: ico-uxf-utilities

%description
Sample homescreen application.

%package devel
Summary:  Development files for %{name}
Group:    Development/GUI/Libraries
Requires: %{name} = %{version}-%{release}
Requires: capi-base-common-devel
Requires: pkgconfig(eina)
Requires: pkgconfig(evas)
Requires: pkgconfig(eina)
Requires: pkgconfig(edje)
Requires: pkgconfig(elementary)
Requires: pkgconfig(ecore-wayland)
Requires: pkgconfig(ecore-x)
Requires: ico-uxf-utilities

%description devel
Development files for application that communicate homescreen.

%prep
%setup -q -n %{name}-%{version}

%define PREFIX /usr/apps/

%build
%autogen

%configure
make %{?_smp_mflags}

%define ico_unitdir_system /usr/lib/systemd/system

%install
rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}/usr/share/applications/
mkdir -p %{buildroot}/usr/share/packages/

# include
mkdir -p %{buildroot}/%{_includedir}/ico-appfw/
cp -f include/ico_apf.h %{buildroot}/%{_includedir}/ico-appfw/
cp -f include/ico_apf_error.h %{buildroot}/%{_includedir}/ico-appfw/
cp -f include/ico_apf_resource_control.h %{buildroot}/%{_includedir}/ico-appfw/
cp -f include/ico_apf_ecore.h %{buildroot}/%{_includedir}/ico-appfw/
cp -f include/ico_apf_log.h %{buildroot}/%{_includedir}/ico-appfw/
cp -f include/ico_uxf_sysdef.h %{buildroot}/%{_includedir}/ico-appfw/

# homescreen
%define APP org.tizen.ico.homescreen
%define APPSDIR %{PREFIX}/%{APP}
mkdir -p %{buildroot}%{APPSDIR}/bin/
mkdir -p %{buildroot}%{APPSDIR}/res/edj
mkdir -p %{buildroot}%{APPSDIR}/res/images
mkdir -p %{buildroot}%{APPSDIR}/res/config
cp -rf data/apps/%{APP} %{buildroot}/%{PREFIX}/
cp -rf res/config %{buildroot}%{APPSDIR}/res/
cp -rf res/apps/%{APP}/* %{buildroot}%{APPSDIR}/res/config/
install -m 0755 src/HomeScreen %{buildroot}%{APPSDIR}/bin/
install -m 0644 src/home_screen_bg.edj %{buildroot}%{APPSDIR}/res/edj
install -m 0644 src/home_screen_touch.edj %{buildroot}%{APPSDIR}/res/edj
install -m 0644 res/images/api_all_off.png %{buildroot}%{APPSDIR}/res/images
install -m 0644 res/images/api_all_on.png %{buildroot}%{APPSDIR}/res/images
install -m 0644 data/share/applications/%{APP}.desktop %{buildroot}/usr/share/applications/
install -m 0644 data/share/packages/%{APP}.xml %{buildroot}/usr/share/packages/

#statusbar
%define APP org.tizen.ico.statusbar
%define APPSDIR %{PREFIX}/%{APP}
mkdir -p %{buildroot}%{APPSDIR}/bin/
mkdir -p %{buildroot}%{APPSDIR}/res/edj
mkdir -p %{buildroot}%{APPSDIR}/res/images
mkdir -p %{buildroot}%{APPSDIR}/res/config
cp -rf data/apps/%{APP} %{buildroot}/%{PREFIX}/
cp -rf res/apps/%{APP}/* %{buildroot}%{APPSDIR}/res/config/
install -m 0755 src/StatusBar %{buildroot}%{APPSDIR}/bin/
install -m 0644 res/images/time*.png %{buildroot}%{APPSDIR}/res/images/
install -m 0644 res/images/applist_*.png %{buildroot}%{APPSDIR}/res/images/
install -m 0644 res/images/home*.png %{buildroot}%{APPSDIR}/res/images/
install -m 0644 data/share/applications/%{APP}.desktop %{buildroot}/usr/share/applications/
install -m 0644 data/share/packages/%{APP}.xml %{buildroot}/usr/share/packages/

#onscreen
%define APP org.tizen.ico.onscreen
%define APPSDIR %{PREFIX}/%{APP}
mkdir -p %{buildroot}%{APPSDIR}/bin/
mkdir -p %{buildroot}%{APPSDIR}/res/edj
mkdir -p %{buildroot}%{APPSDIR}/res/images
mkdir -p %{buildroot}%{APPSDIR}/res/config
cp -rf data/apps/%{APP} %{buildroot}/%{PREFIX}/
cp -rf res/apps/%{APP}/* %{buildroot}%{APPSDIR}/res/config/
install -m 0755 src/OnScreen %{buildroot}%{APPSDIR}/bin/
install -m 0644 src/appli_list.edj %{buildroot}%{APPSDIR}/res/edj/
install -m 0644 src/appli_kill.edj %{buildroot}%{APPSDIR}/res/edj/
install -m 0644 data/share/applications/%{APP}.desktop %{buildroot}/usr/share/applications/
install -m 0644 data/share/packages/%{APP}.xml %{buildroot}/usr/share/packages/

#settings
mkdir -p %{buildroot}/opt/etc/ico/
install -m 0644 settings/mediation_table.txt  %{buildroot}/opt/etc/ico/
mkdir -p %{buildroot}/etc/systemd/system/graphical.target.wants
mkdir -p %{buildroot}%{ico_unitdir_system}/
install -m 0644 settings/ico_homescreen.service %{buildroot}%{ico_unitdir_system}/
ln -sf ../../../../usr/lib/systemd/system/ico_homescreen.service %{buildroot}/etc/systemd/system/graphical.target.wants/

# Update the package database (post only).
%post
mkdir -p /var/log/ico/
chmod 0777 /var/log/ico/
/usr/bin/pkg_initdb
/usr/bin/ail_initdb

%files
%defattr(-,root,root,-)
%{PREFIX}/org.tizen.ico.homescreen
%{PREFIX}/org.tizen.ico.statusbar
%{PREFIX}/org.tizen.ico.onscreen
/usr/share/applications/*.desktop
/usr/share/packages/*.xml
/opt/etc/ico/mediation_table.txt
%{ico_unitdir_system}/ico_homescreen.service
/etc/systemd/system/graphical.target.wants/ico_homescreen.service

%{_libdir}/*.so.*
%{_bindir}/ico_*

%files devel
%defattr(-,root,root,-)
%{_includedir}/ico-appfw/ico_apf.h
%{_includedir}/ico-appfw/ico_apf_error.h
%{_includedir}/ico-appfw/ico_apf_resource_control.h
%{_includedir}/ico-appfw/ico_apf_ecore.h
%{_includedir}/ico-appfw/ico_apf_log.h
%{_includedir}/ico-appfw/ico_uxf_sysdef.h
%{_libdir}/*.so
