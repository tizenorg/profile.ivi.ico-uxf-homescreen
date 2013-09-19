AM_CPPFLAGS = $(GCC_CXXFLAGS) -DLOG_CICO
#AM_CPPFLAGS = $(GCC_CXXFLAGS) -DLOG_CONSOLE
#AM_CPPFLAGS = $(GCC_CXXFLAGS)

lib_LTLIBRARIES =				\
	libico-state-machine.la

libico_state_machine_la_LDFLAGS = -version-info 0:9:0 

libico_state_machine_la_SOURCES =	\
	CicoStateCore.cpp 				\
	CicoEvent.cpp 					\
	CicoEventInfo.cpp 				\
	CicoStateMachine.cpp 			\
	CicoState.cpp 					\
	CicoFinalState.cpp 				\
	CicoHistoryState.cpp 			\
	CicoStateAction.cpp 			\
	CicoStateMachineCreator.cpp 	\
	CicoBlockParser.cpp 			\
	ico_StateMachine_CWrapper.cpp

libico_state_machine_la_CPPFLAGS = 	\
	$(GCC_CXXFLAGS)					\
	-DLOG_CICO						\
    @DLOG_CFLAGS@					\
	-I../../../include				\
	-I/usr/include/ico-util

libico_state_machine_la_LIBADD =	\
    @DLOG_LIBS@

AM_LDFLAGS = -module -avoid-version -rpath $(libdir)

install-exec-hook:
	mkdir -p $(INSTALL_ROOT)/$(includedir)/ico-state-machine
	install -m 644 CicoBlockParser.h $(INSTALL_ROOT)/$(includedir)/ico-state-machine
	install -m 644 CicoEvent.h $(INSTALL_ROOT)/$(includedir)/ico-state-machine
	install -m 644 CicoEventInfo.h $(INSTALL_ROOT)/$(includedir)/ico-state-machine
	install -m 644 CicoFinalState.h $(INSTALL_ROOT)/$(includedir)/ico-state-machine
	install -m 644 CicoHistoryState.h $(INSTALL_ROOT)/$(includedir)/ico-state-machine
	install -m 644 CicoState.h $(INSTALL_ROOT)/$(includedir)/ico-state-machine
	install -m 644 CicoStateAction.h $(INSTALL_ROOT)/$(includedir)/ico-state-machine
	install -m 644 CicoStateCore.h $(INSTALL_ROOT)/$(includedir)/ico-state-machine
	install -m 644 CicoStateMachine.h $(INSTALL_ROOT)/$(includedir)/ico-state-machine
	install -m 644 CicoStateMachineCreator.h $(INSTALL_ROOT)/$(includedir)/ico-state-machine

uninstall:
	rm -rf $(INSTALL_ROOT)/$(includedir)/ico-state-machine
	rm -rf $(INSTALL_ROOT)/$(libdir)/libico-state-machine*

.FORCE :
