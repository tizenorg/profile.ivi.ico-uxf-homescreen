TESTS_ENVIRONMENT = $(SHELL) $(top_srcdir)/tests/apps-framework/run_test.sh

export abs_builddir

AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CFLAGS)

noinst_PROGRAMS =		\
	tst_common  		\
	tst_appresctl  		\
	tst_appresctl_main  \
	tst_inputctl   		\
	tst_userctl    		\
    tst_winctl     		\
	tst_server

check_LTLIBRARIES = $(TESTS)
check_PROGRAMS = tst_common tst_appresctl tst_appresctl_main tst_inputctl tst_userctl tst_winctl tst_server

test_target_flag = -I../../include $(AIL_CFLAGS) $(OPT_CFLAGS) \
                   @ECORE_CFLAGS@ @EINA_CFLAGS@ \
                   -I/usr/include/ico-uxf-weston-plugin     \
                   -I/usr/include/ico-util
test_target_lib = ../../lib/apps-framework/.libs/libico-appfw.so \
                   @ECORE_LIBS@ @EINA_LIBS@ \
                  $(UWS_LIBS) $(OPT_LIBS) $(AIL_LIBS)

tst_common_SOURCES = tst_common.c
tst_common_CFLAGS = $(test_target_flag)
tst_common_LDADD = $(test_target_lib)

tst_appresctl_main_SOURCES = tst_appresctl_main.c
tst_appresctl_main_CFLAGS = $(AIL_CFLAGS)
tst_appresctl_main_LDADD = $(AIL_LIBS)

tst_appresctl_SOURCES = tst_appresctl.c
tst_appresctl_CFLAGS = $(test_target_flag)
tst_appresctl_LDADD = $(test_target_lib)

tst_inputctl_SOURCES = tst_inputctl.c
tst_inputctl_CFLAGS = $(test_target_flag)
tst_inputctl_LDADD = $(test_target_lib)

tst_userctl_SOURCES = tst_userctl.c
tst_userctl_CFLAGS = $(test_target_flag)
tst_userctl_LDADD = $(test_target_lib)

tst_winctl_SOURCES = tst_winctl.c
tst_winctl_CFLAGS = $(test_target_flag)
tst_winctl_LDADD = $(test_target_lib)

tst_server_SOURCES = tst_server.c
tst_server_CFLAGS = $(test_target_flag)
tst_server_LDADD = $(test_target_lib)

EXTRA_DIST = run_test.sh
