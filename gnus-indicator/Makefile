GTK_LIBS:=`pkg-config --cflags --libs gtk+-2.0`
APPINDICATOR_LIBS:=`pkg-config --cflags --libs appindicator-0.1`

CC=gcc
CFLAGS=-Wall

all: bin/gnus-indicator-d bin/gnus_indicator_t00 bin/gnus_indicator_t01 bin/gnus_indicator_t02


bin/gnus-indicator-d:  src/gnus-indicator-d.c
	$(CC) $(CFLAGS) -o $@ $^ $(GTK_LIBS) $(APPINDICATOR_LIBS)


bin/gnus_indicator_t00:  src/gnus_indicator_t00.c
	$(CC) $(CFLAGS) -o $@ $^ $(GTK_LIBS) $(APPINDICATOR_LIBS)


bin/gnus_indicator_t01:  src/gnus_indicator_t01.c
	$(CC) $(CFLAGS) -o $@ $^ $(GTK_LIBS) $(APPINDICATOR_LIBS)


bin/gnus_indicator_t02:  src/gnus_indicator_t02.c
	$(CC) $(CFLAGS) -o $@ $^ $(GTK_LIBS) $(APPINDICATOR_LIBS)


clean:
	rm -f bin/*

install:
	cp bin/gnus-indicator-d /usr/bin/
	cp scripts/gnus-indicator /usr/bin/
	cp icons/gnus_indicator_*.png /usr/share/pixmaps/
