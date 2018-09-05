CXX = g++
CXXFLAGS = -std=c++11 -g -pthread
LD = $(CXX)
LDFLAGS = -lpthread

all : Heracles

Heracles : TopLevelController.o CoreMemController.o Cores.o LLC.o Util.o 
	$(LD) $(LDFLAGS) -o $@ $^

TopLevelController.o : TopLevelController.cc CoreMemController.h Task.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

CoreMemController.o : CoreMemController.cc CoreMemController.h Cores.h LLC.h Task.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

Cores.o : Cores.cc Cores.h Util.h Task.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

LLC.o : LLC.cc LLC.h Util.h Task.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

Util.o : Util.cc Util.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean :
	rm -f *.o
	rm -f Heracles
