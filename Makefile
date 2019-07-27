all: libcityinfo0.so.0.0.0

libcityinfo0.so.0.0.0: cityinfo.c cityinfo.h
	$(CC) $(CFLAGS) $(shell pkg-config --cflags --libs glib-2.0 gconf-2.0) -shared -fPIC -Wl,-soname=libcityinfo0.so.0 $^ -o $@

test: test.c
	ln -sf libcityinfo0.so.0.0.0 libcityinfo0.so
	$(CC) $(CFLAGS) $(shell pkg-config --cflags --libs glib-2.0) -I. -L. -lcityinfo0 $^ -o $@
clean:
	rm -f libcityinfo0.so.0.0.0 libcityinfo0.so test

install:
	install -d "$(DESTDIR)/usr/lib/"
	install -m 755 libcityinfo0.so.0.0.0 "$(DESTDIR)/usr/lib/"
	ln -s libcityinfo0.so.0.0.0 "$(DESTDIR)/usr/lib/libcityinfo0.so.0"

	install -d "$(DESTDIR)/usr/include/"
	install -d  "$(DESTDIR)/usr/lib/pkgconfig"
	ln -s libcityinfo0.so.0 "$(DESTDIR)/usr/lib/libcityinfo0.so"
	install -m 644 cityinfo.h "$(DESTDIR)/usr/include/"
	install -m 644 libcityinfo0-0.pc "$(DESTDIR)/usr/lib/pkgconfig"

	install -d "$(DESTDIR)/usr/share/clock"
	install -m 644 data/wdb "$(DESTDIR)/usr/share/clock/wdb"
