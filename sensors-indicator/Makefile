GTK_LIBS:=`pkg-config --cflags --libs gtk+-2.0`
APPINDICATOR_LIBS:=`pkg-config --cflags --libs appindicator-0.1`

CXX=g++
CXXFLAGS=-std=c++11 -Wall

all: bin/sensors-indicator-d bin/sensors_indicator_t00 bin/sensors_indicator_t01 bin/sensors_indicator_t02


bin/sensors-indicator-d:  src/sensors-indicator-d.c
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTK_LIBS) $(APPINDICATOR_LIBS)


bin/sensors_indicator_t00:  src/sensors_indicator_t00.c
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTK_LIBS) $(APPINDICATOR_LIBS)


bin/sensors_indicator_t01:  src/sensors_indicator_t01.c
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTK_LIBS) $(APPINDICATOR_LIBS)


bin/sensors_indicator_t02:  src/sensors_indicator_t02.c
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTK_LIBS) $(APPINDICATOR_LIBS)


clean:
	rm -f bin/*

install:
	cp bin/sensors-indicator-d /usr/bin/
	cp icons/sensors_*.svg /usr/share/pixmaps/
