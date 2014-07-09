Name:       ico-uxf-homescreen
Summary:    ICO homescreen application
Version:    0.9.31
Release:    1.1
Group:		Graphics & Automotive UI
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
BuildRequires: pkgconfig(murphy-common)
BuildRequires: pkgconfig(murphy-domain-controller)
BuildRequires: pkgconfig(murphy-ecore)
BuildRequires: pkgconfig(murphy-resource)
#BuildRequires: pkgconfig(murphy-system-controller)
BuildRequires: pkgconfig(notification)
BuildRequires: pkgconfig(appsvc)
BuildRequires: boost-devel
BuildRequires: mesa-devel
BuildRequires: ico-uxf-utilities-devel >= 0.9.07
Requires: weston >= 1.5
#Requires: murphy-system-controller
Requires: ico-uxf-utilities >= 0.9.07

%description
ICO homescreen application

%prep
%setup -q -n %{name}-%{version}

%build
%autogen
make %{?_smp_mflags}

%install
rm -rf %{buildroot}

%make_install

%post
/sbin/ldconfig

# Update the app database.
%{_bindir}/pkginfo --imd /usr/share/packages/org.tizen.ico.homescreen.xml
%{_bindir}/pkginfo --imd /usr/share/packages/org.tizen.ico.statusbar.xml
%{_bindir}/pkginfo --imd /usr/share/packages/org.tizen.ico.onscreen.xml

%preun
# Update the app database.
%{_bindir}/pkginfo --rmd /usr/share/packages/org.tizen.ico.homescreen.xml
%{_bindir}/pkginfo --rmd /usr/share/packages/org.tizen.ico.statusbar.xml
%{_bindir}/pkginfo --rmd /usr/share/packages/org.tizen.ico.onscreen.xml

%postun
/sbin/ldconfig
rm -f /usr/share/applications/org.tizen.ico.homescreen.desktop
rm -f /usr/share/applications/org.tizen.ico.statusbar.desktop
rm -f /usr/share/applications/org.tizen.ico.onscreen.desktop

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.ico.homescreen
/usr/apps/org.tizen.ico.statusbar
/usr/apps/org.tizen.ico.onscreen
/usr/share/packages/org.tizen.ico.homescreen.xml
/usr/share/packages/org.tizen.ico.statusbar.xml
/usr/share/packages/org.tizen.ico.onscreen.xml
