CC=i586-mingw32msvc-gcc
CXX=i586-mingw32msvc-g++
LD=i586-mingw32msvc-ld
RES=i586-mingw32msvc-windres
CXXFLAGS=-Wall -Werror -g -I include

OBJECTS=bin/TrayAlive.o \
		bin/IpPacket.o \
		bin/IcmpPacket.o \
		bin/Ping.o \
		bin/TrayIcon.o \
		bin/NotifyWindowClass.o \
		bin/TrayAliveResources.o

all: TrayAlive.exe

distrib: TrayAlive.exe InstallTrayAlive.exe

InstallTrayAlive.exe:
	makensis src/setup.nsi

TrayAlive.exe: $(OBJECTS)
	$(CXX) -Wl,--subsystem=windows -o bin/$@ $(OBJECTS) -lws2_32

bin/TrayAliveResources.o: src/TrayAlive.rc
	$(RES) -I include -I data $< $@

bin/Ping.o: src/Ping.cpp include/Ping.h include/PingException.h include/IcmpPacket.h include/IpPacket.h
	$(CXX) $(CXXFLAGS) -c -o $@ src/Ping.cpp

bin/NotifyWindowClass.o: src/NotifyWindowClass.cpp include/NotifyWindowClass.h
	$(CXX) $(CXXFLAGS) -c -o $@ src/NotifyWindowClass.cpp

bin/TrayAlive.o: src/TrayAlive.cpp include/Ping.h include/NotifyWindowClass.h include/PingException.h include/TrayIcon.h
	$(CXX) $(CXXFLAGS) -c -o $@ src/TrayAlive.cpp

bin/IcmpPacket.o: src/IcmpPacket.cpp include/IcmpPacket.h
	$(CXX) $(CXXFLAGS) -c -o $@ src/IcmpPacket.cpp

bin/IpPacket.o: src/IpPacket.cpp include/IpPacket.h
	$(CXX) $(CXXFLAGS) -c -o $@ src/IpPacket.cpp

bin/TrayIcon.o: src/TrayIcon.cpp include/TrayIcon.h
	$(CXX) $(CXXFLAGS) -c -o $@ src/TrayIcon.cpp

clean:
	rm -rf bin/*

