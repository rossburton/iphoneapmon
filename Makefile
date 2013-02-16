all: test Ap-1.0.typelib

test: test.c libapmonitor.so
	$(LINK.c) -o $@ $^ -g `pkg-config --cflags --libs gobject-2.0`


libapmonitor.so: ap-monitor.c ap-monitor.h marshals.c marshals.h
	$(LINK.c) -o $@ $^ -g -shared -fPIC `pkg-config --cflags --libs gobject-2.0 avahi-client avahi-glib`

%.c: %.list
	glib-genmarshal --prefix=ap_marshal --internal --body $^ > $@

%.h: %.list
	glib-genmarshal --prefix=ap_marshal --internal --header $^ > $@

Ap-1.0.gir: libapmonitor.so
	g-ir-scanner ap-monitor.c ap-monitor.h -n Ap --nsversion=1.0 --library apmonitor --pkg=gobject-2.0,avahi-gobject -i GObject-2.0 -L `pwd` -o $@

%.typelib: %.gir
	g-ir-compiler $^ -o $@

PREFIX=${HOME}/Programming/MeeGo/netbook/install/

install: Ap-1.0.typelib
	install -m755 libapmonitor.so ${PREFIX}/lib
	install -m644 Ap-1.0.gir ${PREFIX}/share/gir-1.0/
	install -m644 Ap-1.0.typelib ${PREFIX}/lib/girepository-1.0/
