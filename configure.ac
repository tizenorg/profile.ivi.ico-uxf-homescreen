#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([org.tizen.ico.homescreen], [0.7.03], [])
AM_INIT_AUTOMAKE([foreign 1.11])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Initialize libtool
LT_PREREQ([2.2])
LT_INIT([disable-static])

# Checks for libraries.
PKG_PROG_PKG_CONFIG()

AC_CHECK_LIB([aul], [aul_app_get_appid_bypid], AUL_LIBS="-laul")
AC_SUBST(AUL_LIBS)

PKG_CHECK_MODULES([EINA], [eina])
PKG_CHECK_MODULES([EVAS], [evas])
PKG_CHECK_MODULES([ECORE], [ecore])
PKG_CHECK_MODULES([EDJE], [edje])
PKG_CHECK_MODULES([ELEM], [elementary])
PKG_CHECK_MODULES([ECOREWAYLAND], [ecore-wayland])
PKG_CHECK_MODULES([ECOREX], [ecore-x])
EFL_CFLAGS="$EINA_CFLAGS $EVAS_CFLAGS $ECORE_CFLAGS $EDJE_CFLAGS $ELEM_CFLAGS $ECOREWAYLAND_CFLAGS"
EFL_LIBS="$EINA_LIBS $EVAS_LIBS $ECORE_LIBS $EDJE_LIBS $ELEM_LIBS $ECOREWAYLAND_LIBS $ECOREX_LIBS"
AC_SUBST(EFL_CFLAGS)
AC_SUBST(EFL_LIBS)

PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([DBUS], [dbus-1])
PKG_CHECK_MODULES([JSONGLIB], [json-glib-1.0])
OPT_CFLAGS="$GLIB_CFLAGS $JSONGLIB_CFLAGS"
OPT_LIBS="$GLIB_LIBS $JSONGLIB_LIBS"
AC_SUBST(OPT_CFLAGS)
AC_SUBST(OPT_LIBS)

PKG_CHECK_MODULES([AUL], [aul])
PKG_CHECK_MODULES([BUNDLE], [bundle])
PKG_CHECK_MODULES([AIL], [ail])
AUL_CFLAGS="$AUL_CFLAGS $AIL_CFLAGS $BUNDLE_CFLAGS"
AUL_LIBS="$AUL_LIBS $AIL_LIBS $BUNDLE_LIBS"
AC_SUBST(AUL_CFLAGS)
AC_SUBST(AUL_LIBS)

UWS_CFLAGS="-I/usr/include/ico-util"
UWS_LIBS="-lico-util-com"
AC_SUBST(UWS_CFLAGS)
AC_SUBST(UWS_LIBS)

PKG_CHECK_MODULES([SLPPKGMGR], [pkgmgr])
SLPPKGMGR_CFLAGS="$SLPPKGMGR_CFLAGS"
SLPPKGMGR_LIBS="$SLPPKGMGR_LIBS"
AC_SUBST(SLPPKGMGR_CFLAGS)
AC_SUBST(SLPPKGMGR_LIBS)

PKG_CHECK_MODULES([WAYLANDCLIENT], [wayland-client])
PKG_CHECK_MODULES([WAYLANDCURSOR], [wayland-cursor])
WL_CFLAGS="$WAYLANDCLIENT_CFLAGS $WAYLANDCURSOR_CFLAGS"
WL_LIBS="$WAYLANDCLIENT_LIBS $WAYLANDCURSOR_LIBS"
AC_SUBST(WL_CFLAGS)
AC_SUBST(WL_LIBS)

if test "x$GCC" = "xyes"; then
    my_common_gcc_flags="-Wall -Wextra -Wno-unused-parameter \
        -Wno-missing-field-initializers -g -fvisibility=hidden"
    GCC_CFLAGS="$my_common_gcc_flags \
        -Wstrict-prototypes -Wmissing-prototypes"
    GCC_CXXFLAGS="$my_common_gcc_flags"
fi
AC_SUBST(GCC_CFLAGS)
AC_SUBST(GCC_CXXFLAGS)

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h strings.h sys/file.h sys/ioctl.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gethostname gettimeofday memset setenv strcasecmp strdup strncasecmp strtol tzset])

AC_CONFIG_FILES([Makefile
				 ico-app-framework/Makefile
                 src/Makefile
                 apps_controller/Makefile
                 test/Makefile])
AC_PROG_RANLIB([ranlib])
AC_OUTPUT
