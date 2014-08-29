#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([org.tizen.ico.homescreen], [0.9.31], [])
AM_INIT_AUTOMAKE([1.11 foreign no-dist-gzip dist-xz])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])

AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Initialize libtool
LT_PREREQ([2.2])
#LT_INIT([disable-static])
LT_INIT([static])

# Checks for libraries.
PKG_PROG_PKG_CONFIG()

AC_CHECK_LIB([aul], [aul_app_get_appid_bypid], AUL_LIBS="-laul")
AC_SUBST(AUL_LIBS)

# libraries for common.
PKG_CHECK_MODULES([EGL], [egl])
PKG_CHECK_MODULES([EINA], [eina])
PKG_CHECK_MODULES([EVAS], [evas])
PKG_CHECK_MODULES([ECORE], [ecore])
PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([JSONGLIB], [json-glib-1.0])
PKG_CHECK_MODULES([AIL], [ail])
PKG_CHECK_MODULES([BUNDLE], [bundle])
PKG_CHECK_MODULES([AUL], [aul])
PKG_CHECK_MODULES([PKGMGR], [pkgmgr])
PKG_CHECK_MODULES([WAYLANDCLIENT], [wayland-client])
PKG_CHECK_MODULES([NOTIFICATION], [notification])
PKG_CHECK_MODULES([APPSVC], [appsvc])

# libraries for homescreen.
PKG_CHECK_MODULES([EDJE], [edje])
PKG_CHECK_MODULES([ECOREWAYLAND], [ecore-wayland])
PKG_CHECK_MODULES([WAYLANDCURSOR], [wayland-cursor])

# libraries for system-controller.
PKG_CHECK_MODULES([DLOG], [dlog])
PKG_CHECK_MODULES([CAPIAPPFWAPPLICATION], [capi-appfw-application])
PKG_CHECK_MODULES([MURPHYCOMMON], [murphy-common]);
PKG_CHECK_MODULES([MURPHYDOMAINCONTROLLER], [murphy-domain-controller]);
PKG_CHECK_MODULES([MURPHYECORE], [murphy-ecore]);
PKG_CHECK_MODULES([MURPHYRESOURCE], [murphy-resource]);
PKG_CHECK_MODULES([LIBWEBSOCKETS], [libwebsockets]);
PKG_CHECK_MODULES([ECOREEVAS], [ecore-evas])

# Set flags
EFL_CFLAGS="$EINA_CFLAGS $EVAS_CFLAGS $ECORE_CFLAGS $EDJE_CFLAGS $ECOREWAYLAND_CFLAGS"
EFL_LIBS="$EINA_LIBS $EVAS_LIBS $ECORE_LIBS $EDJE_LIBS $ECOREWAYLAND_LIBS $ECOREX_LIBS"
AC_SUBST(EFL_CFLAGS)
AC_SUBST(EFL_LIBS)

OPT_CFLAGS="$DLOG_CFLAGS $GLIB_CFLAGS $JSONGLIB_CFLAGS"
OPT_LIBS="$DLOG_LIBS $GLIB_LIBS $JSONGLIB_LIBS"
AC_SUBST(OPT_CFLAGS)
AC_SUBST(OPT_LIBS)

AUL_CFLAGS="$AUL_CFLAGS $AIL_CFLAGS $BUNDLE_CFLAGS"
AUL_LIBS="$AUL_LIBS $AIL_LIBS $BUNDLE_LIBS"
AC_SUBST(AUL_CFLAGS)
AC_SUBST(AUL_LIBS)

UWS_CFLAGS="-I/usr/include/ico-util"
UWS_LIBS="-lico-util"
AC_SUBST(UWS_CFLAGS)
AC_SUBST(UWS_LIBS)

GENIVI_LIBS="-lilmCommon -lilmControl -lilmClient"
AC_SUBST(GENIVI_LIBS)

SLPPKGMGR_CFLAGS="$SLPPKGMGR_CFLAGS"
SLPPKGMGR_LIBS="$SLPPKGMGR_LIBS"
AC_SUBST(SLPPKGMGR_CFLAGS)
AC_SUBST(SLPPKGMGR_LIBS)

WL_CFLAGS="$WAYLANDCLIENT_CFLAGS $WAYLANDCURSOR_CFLAGS"
WL_LIBS="$WAYLANDCLIENT_LIBS $WAYLANDCURSOR_LIBS"
AC_SUBST(WL_CFLAGS)
AC_SUBST(WL_LIBS)

CFLAGS="$CFLAGS $EGL_CFLAGS"
AC_SUBST(CFLAGS)

if test "x$GCC" = "xyes"; then
    my_common_gcc_flags="-Wall -Wextra -Wno-unused-parameter \
        -Wno-missing-field-initializers -g -fvisibility=hidden"
    GCC_CFLAGS="$my_common_gcc_flags \
        -Wstrict-prototypes -Wmissing-prototypes"
	GCC_CXXFLAGS="-Wall -Wextra -Wno-unused-parameter \
		-Wno-missing-field-initializers -g"
fi
AC_SUBST(GCC_CFLAGS)
AC_SUBST(GCC_CXXFLAGS)

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h strings.h sys/file.h sys/ioctl.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_INT32_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([gethostname gettimeofday memset setenv strcasecmp strdup strerror strncasecmp strtol strtoul tzset])

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 lib/common/Makefile
                 lib/notification/Makefile
                 lib/apps-framework/Makefile
                 lib/misc/Makefile
                 lib/misc/state-machine/Makefile
                 lib/system-controller/Makefile
                 src/Makefile
                 src/syscond/Makefile
                 src/homescreen/Makefile
                 src/statusbar/Makefile
                 src/onscreen/Makefile
                 tool/Makefile
                 tests/Makefile
                 tests/system-controller/Makefile
                 tests/system-controller/apps-framework/Makefile])
AC_PROG_RANLIB([ranlib])
AC_OUTPUT
