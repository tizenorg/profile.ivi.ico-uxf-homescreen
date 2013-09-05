Name:       ico-uxf-homescreen
Summary:    Sample homescreen and system controller
Version:    0.9.01
Release:    1.3
Group:		Graphics & UI Framework/Automotive UI
License:    Apache-2.0
URL:        ""
Source0:    %{name}-%{version}.tar.bz2

BuildRequires: pkgconfig(wayland-client) >= 1.2
BuildRequires: ico-uxf-weston-plugin-devel >= 0.7.01
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(ecore-wayland)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(edje)
BuildRequires: pkgconfig(elementary)
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
BuildRequires: boost-devel
BuildRequires: ico-uxf-utilities-devel >= 0.2.01
Requires: weston >= 1.2
Requires: ico-uxf-weston-plugin >= 0.7.01
Requires: ico-uxf-utilities >= 0.2.01

%description
sample homescreen & system controller.

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

HOMESCREENDIR="%{buildroot}/usr/apps/org.tizen.ico.homescreen"
IMGDIR="res/org.tizen.ico.homescreen/res/images"
mkdir -p ${HOMESCREENDIR}/bin
mkdir -p ${HOMESCREENDIR}/var
mkdir -p ${HOMESCREENDIR}/res/images
mkdir -p ${HOMESCREENDIR}/res/edj
mkdir -p ${HOMESCREENDIR}/res/config
mkdir -p %{buildroot}/bin
mkdir -p %{buildroot}/usr/share/applications
mkdir -p %{buildroot}/usr/share/packages
cp -fr res/org.tizen.ico.homescreen/res/config ${HOMESCREENDIR}/res
cp -fr data/apps/org.tizen.ico.homescreen %{buildroot}/usr/apps/
cp src/homescreen/HomeScreen ${HOMESCREENDIR}/bin
cp src/homescreen/home_screen_bg.edj ${HOMESCREENDIR}/res/edj
cp src/homescreen/home_screen_touch.edj ${HOMESCREENDIR}/res/edj
chmod 666 ${HOMESCREENDIR}/res/edj/home_screen_*
cp ${IMGDIR}/bg.png ${HOMESCREENDIR}/res/images
cp ${IMGDIR}/ctrl.png ${HOMESCREENDIR}/res/images
cp ${IMGDIR}/applist_off.png ${HOMESCREENDIR}/res/images
cp ${IMGDIR}/api_all_off.png ${HOMESCREENDIR}/res/images
cp ${IMGDIR}/api_all_on.png ${HOMESCREENDIR}/res/images
cp ${IMGDIR}/pagePointer_n.png ${HOMESCREENDIR}/res/images
cp ${IMGDIR}/pagePointer_p.png ${HOMESCREENDIR}/res/images
cp ${IMGDIR}/button_really.png ${HOMESCREENDIR}/res/images
cp ${IMGDIR}/button_no.png ${HOMESCREENDIR}/res/images
cp ${IMGDIR}/button_yes.png ${HOMESCREENDIR}/res/images
cp ${IMGDIR}/termIcon.png ${HOMESCREENDIR}/res/images
cp ${IMGDIR}/tizen_32.png ${HOMESCREENDIR}/res/images
chmod 666 ${HOMESCREENDIR}/res/images/api_all_*
cp data/share/packages/org.tizen.ico.homescreen.xml %{buildroot}/usr/share/packages
cp res/org.tizen.ico.homescreen/res/apps/org.tizen.ico.homescreen/* ${HOMESCREENDIR}/res/config

STATUSBARDIR="%{buildroot}/usr/apps/org.tizen.ico.statusbar"
mkdir -p ${STATUSBARDIR}/res/images
mkdir -p ${STATUSBARDIR}/res/edj
mkdir -p ${STATUSBARDIR}/res/config
mkdir -p ${STATUSBARDIR}/bin
cp -rf data/apps/org.tizen.ico.statusbar %{buildroot}/usr/apps/
cp src/homescreen/StatusBar ${STATUSBARDIR}/bin/
cp res/org.tizen.ico.homescreen/res/images/time*.png ${STATUSBARDIR}/res/images
cp res/org.tizen.ico.homescreen/res/images/applist_*.png ${STATUSBARDIR}/res/images
cp res/org.tizen.ico.homescreen/res/images/home*.png ${STATUSBARDIR}/res/images
chmod 666 ${STATUSBARDIR}/res/images/time*.png
cp data/share/packages/org.tizen.ico.statusbar.xml %{buildroot}/usr/share/packages
cp res/org.tizen.ico.homescreen/res/apps/org.tizen.ico.statusbar/* ${STATUSBARDIR}/res/config
#make install prefix=%{buildroot}/usr

%post
/sbin/ldconfig
mkdir -p %{_localstatedir}/log/ico/
chmod 0777 %{_localstatedir}/log/ico/

# Update the app database.
%{_bindir}/pkg_initdb
%{_bindir}/ail_initdb

%preun

%postun
/sbin/ldconfig
rm -f /usr/share/applications/org.tizen.ico.homescreen.desktop
rm -f /usr/share/applications/org.tizen.ico.statusbar.desktop
rm -f /usr/share/applications/org.tizen.ico.system-controller.desktop
rm -f /home/app/layout.txt

# Update the app database.
%{_bindir}/pkg_initdb
%{_bindir}/ail_initdb

%files
%defattr(-,root,root,-)
/usr/apps/org.tizen.ico.homescreen
/usr/apps/org.tizen.ico.statusbar
/usr/share/packages/org.tizen.ico.homescreen.xml
/usr/share/packages/org.tizen.ico.statusbar.xml
%{_libdir}/libico-appfw.*
%{_libdir}/libico-state-machine.*
/usr/apps/org.tizen.ico.system-controller
/usr/share/packages/org.tizen.ico.system-controller.xml
/usr/lib/systemd/user/ico-system-controller.service
/usr/lib/systemd/user/ico-uxf-wait-launchpad-ready.path
/usr/lib/systemd/user/weston.target.wants/ico-uxf-wait-launchpad-ready.path

%files system-controller-devel
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
%{_libdir}/libico-appfw.*
%{_libdir}/libico-state-machine.*
