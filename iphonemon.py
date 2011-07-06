#! /usr/bin/env python

# Copyright (C) 2011 Intel Corp
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.
#
# Author: Ross Burton <ross.burton@intel.com>

import avahi, dbus, gobject

from dbus.mainloop.glib import DBusGMainLoop
loop = DBusGMainLoop(set_as_default=True)

bus = dbus.SystemBus()
avahi_daemon = dbus.Interface(bus.get_object(avahi.DBUS_NAME,
                                             avahi.DBUS_PATH_SERVER),
                              avahi.DBUS_INTERFACE_SERVER)

# Hash of service names (i.e. "Ross Burton's iPhone" to resolver objects
resolvers = {}

def on_resolved(interface, protocol, name, stype, domain, host, aprotocol, address, port, txt, flags):
    # 1-5 (no signal is tech=N/A)
    strength = None
    # N/A, 3G, 3_75G
    technology = None
    for l in txt:
        (k,v) = str(l).split("=", 1)
        if k == "ss":
            strength = ord(v)
        elif k == "di":
            technology = v
    print strength, technology

def on_error(message):
    print message

def on_new_service(interface, protocol, name, stype, domain, flags):
    print "Found %s" % name

    path = avahi_daemon.ServiceResolverNew(interface, protocol, name, stype, domain,
                                    avahi.PROTO_UNSPEC, dbus.UInt32(0))
    resolver = dbus.Interface(bus.get_object(avahi.DBUS_NAME, path),
                              avahi.DBUS_INTERFACE_SERVICE_RESOLVER)

    resolver.connect_to_signal("Found", on_resolved, byte_arrays=True)
    resolver.connect_to_signal("Failure", on_error)

    resolvers[name] = resolver

def on_remove_service(interface, protocol, name, type, domain, flags):
    print "Lost %s" % name

    resolver = resolvers.pop(name)
    resolver.Free()

def search():
    browser = dbus.Interface(bus.get_object(avahi.DBUS_NAME,
                                            avahi_daemon.ServiceBrowserNew(avahi.IF_UNSPEC,
                                                                           avahi.PROTO_UNSPEC,
                                                                           "_link411._udp",
                                                                           "",
                                                                           dbus.UInt32(0))),
                             avahi.DBUS_INTERFACE_SERVICE_BROWSER)
    browser.connect_to_signal('ItemNew', on_new_service)
    browser.connect_to_signal('ItemRemove', on_remove_service)


if __name__ == "__main__":
    search()
    gobject.MainLoop().run()
