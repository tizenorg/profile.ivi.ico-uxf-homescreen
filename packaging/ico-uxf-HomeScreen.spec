Name:       ico-uxf-HomeScreen
Summary:    sample homescreen
Version:    0.3.07
Release:    1.1
Group:		TO_BE/FILLED_IN
License:    Apache License, Version 2.0
URL:        ""
Source0:    %{name}-%{version}.tar.bz2

BuildRequires: pkgconfig(wayland-client) >= 1.0
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: ico-uxf-weston-plugin-devel >= 0.5.05
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
Requires: weston >= 1.0
Requires: ico-uxf-weston-plugin >= 0.5.05

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

%description devel
Development files for application that communicate homescreen.

%prep
%setup -q -n %{name}-%{version}

%define PREFIX /opt/apps/

%build
autoreconf --install

%autogen

%configure
make %{?_smp_mflags}

%global ico_sysvlinkdir %{_sysconfdir}/rc.d/rc3.d

%install
rm -rf %{buildroot}
%make_install

%define ictl_conf /opt/etc/ico-uxf-device-input-controller
mkdir -p %{buildroot}/opt/share/applications/

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
install -m 0644 data/share/applications/%{APP}.desktop %{buildroot}/opt/share/applications/

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
install -m 0644 data/share/applications/%{APP}.desktop %{buildroot}/opt/share/applications/

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
install -m 0644 data/share/applications/%{APP}.desktop %{buildroot}/opt/share/applications/

#settings
mkdir -p %{buildroot}/opt/etc/ico
install -m 0644 settings/mediation_table.txt  %{buildroot}/opt/etc/ico/
mkdir -p %{buildroot}%{_sysconfdir}/rc.d/init.d
mkdir -p %{buildroot}%{ico_sysvlinkdir}
install -m 0755 settings/ico_weston  %{buildroot}%{_sysconfdir}/rc.d/init.d/
ln -sf %{_sysconfdir}/rc.d/init.d/ico_weston %{buildroot}%{ico_sysvlinkdir}/S91ico_weston

%global ico_packagestatedir %{_localstatedir}/lib/rpm-state/%{name}
%post
# The homescreen boot script will start Weston in a specific order.
# Disable stand-alone boot of Weston by removing the link to its boot
# script in /etc/rc.d/rc3.d.  Store it for later restoration after
# uninstallation of this package.
mkdir -p %{ico_packagestatedir} > /dev/null 2>&1
mv -f %{ico_sysvlinkdir}/S??weston %{ico_packagestatedir} > /dev/null 2>&1
set $?=0

# Update the app database.
rm -f /opt/dbspace/.app_info.db*
ail_initdb

%postun
# Restore the link to the Weston boot script.
mv %{ico_packagestatedir}/S??weston %{ico_sysvlinkdir} > /dev/nukk 2>&1
rm -fr %{ico_packagestatedir}
set $?=0

# Update the app database.
rm -f /opt/dbspace/.app_info.db*
ail_initdb

%files
%defattr(-,root,root,-)
%{PREFIX}/org.tizen.ico.homescreen
%{PREFIX}/org.tizen.ico.statusbar
%{PREFIX}/org.tizen.ico.onscreen
/opt/share/applications/*.desktop

%{_libdir}/*.so.*
%{_bindir}/ico_*

/opt/etc/ico/mediation_table.txt
%{_sysconfdir}/rc.d/init.d/ico_weston
%{ico_sysvlinkdir}/S91ico_weston

%files devel
%defattr(-,root,root,-)
%{_includedir}/ico-appfw/ico_apf.h
%{_includedir}/ico-appfw/ico_apf_error.h
%{_includedir}/ico-appfw/ico_apf_resource_control.h
%{_includedir}/ico-appfw/ico_apf_ecore.h
%{_includedir}/ico-appfw/ico_apf_log.h
%{_includedir}/ico-appfw/ico_uxf_sysdef.h
%{_libdir}/*.so
