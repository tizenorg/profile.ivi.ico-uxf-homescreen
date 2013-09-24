INC=-I/usr/include/notification
LIBDIR=-L./
LIB=-lnotification
ECOREFLAGS=-I/usr/include/ecore-1 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/eina-1 -I/usr/include/eina-1/eina
ECORELIBS=-lecore -leina -lpthread
LIBSRC=CIcoNotification.cc CIcoNotificationService.cc CIcoNotificationApp.cc
BIN=AppNotification ServiceNotification NotificationTest 

all:$(BIN) libiconotification

AppNotification:app.cc libiconotification
	g++ -c -g $(INC) app.cc
	g++ -o appnotification -g $(LIBDIR) -liconotification app.o

ServiceNotification:service.cc libiconotification
	g++ -c -g $(INC) $(ECOREFLAGS) service.cc
	g++ -o servicenotification -g $(LIBDIR) $(ECORELIBS) $(LIB) -liconotification service.o

NotificationTest:test.cc libiconotification 
	g++ -c -g $(INC) test.cc
	g++ -o test -g $(INC) $(LIBDIR) -liconotification test.o

libiconotification:$(LIBSRC)
	g++ -fPIC -Wall -g $(INC) $(LIB) -shared $(LIBSRC) -o libiconotification.so
clean:
	rm -fr *.o *.so $(BIN)
