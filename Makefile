test: test.c libapmonitor.so
	$(LINK.c) -o $@ $^ -g `pkg-config --cflags --libs gobject-2.0`

libapmonitor.so: ap-monitor.c ap-monitor.h
	$(LINK.c) -o $@ $^ -g -shared -fPIC `pkg-config --cflags --libs gobject-2.0 avahi-gobject`


Ap.gir: libapmonitor.so
	g-ir-scanner ap-monitor.c ap-monitor.h -n Ap --nsversion=1.0 --library apmonitor --pkg=gobject-2.0,avahi-gobject -i GObject-2.0 -o $@

%.typelib: %.gir
	g-ir-compiler $^ -o $@

PREFIX=${HOME}/Programming/MeeGo/netbook/install/

install: Ap.typelib
	install -m755 libapmonitor.so ${PREFIX}/lib64
	install -m644 Ap.gir ${PREFIX}/share/gir-1.0/
	install -m644 Ap.typelib ${PREFIX}/lib64/girepository-1.0/
