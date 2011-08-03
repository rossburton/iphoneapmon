const DBus = imports.dbus;
const Mainloop = imports.mainloop;

/*
 * Avahi Server object
 */

var AvahiServerIface = {
  name: "org.freedesktop.Avahi.Server",
  methods: [
    {
      name: "GetVersionString",
      inSignature: "",
      outSignature: "s"
    },
    {
      name: "GetAPIVersion",
      inSignature: "",
      outSignature: "u"
    },
    {
      name: "GetHostName",
      inSignature: '',
      outSignature: 's'
    },
    {
      name: "SetHostName",
      inSignature: "s",
      outSignature: ""
    },
    {
      name: "GetHostNameFqdn",
      inSignature: "",
      outSignature: "s"
    },
    {
      name: "GetDomainName",
      inSignature: "",
      outSignature: "s"
    },
    {
      name: "IsNSSSupportAvailable",
      inSignature: "",
      outSignature: "b"
    },
    {
      name: "GetState",
      inSignature: "",
      outSignature: "i"
    },
    {
      name: "GetLocalServiceCookie",
      inSignature: "",
      outSignature: "u"
    },
    {
      name: "GetAlternativeHostName",
      inSignature: "s",
      outSignature: "s"
    },
    {
      name: "GetAlternativeServiceName",
      inSignature: "s",
      outSignature: "s"
    },
    {
      name: "GetNetworkInterfaceNameByIndex",
      inSignature: "i",
      outSignature: "s"
    },
    {
      name: "GetNetworkInterfaceIndexByName",
      inSignature: "s",
      outSignature: "i"
    },
    {
      name: "ResolveHostName",
      inSignature: "iisiu",
      outSignature: "iisisu"
    },
    {
      name: "ResolveAddress",
      inSignature: "iisu",
      outSignature: "iiissu"
    },
    {
      name: "ResolveService",
      inSignature: "iisssiu",
      outSignature: "iissssisqaayu"
    },
    {
      name: "EntryGroupNew",
      inSignature: "",
      outSignature: "o"
    },
    {
      name: "DomainBrowserNew",
      inSignature: "iisiu",
      outSignature: "o"
    },
    {
      name: "ServiceTypeBrowserNew",
      inSignature: "iisu",
      outSignature: "o"
    },
    {
      name: "ServiceBrowserNew",
      inSignature: "iissu",
      outSignature: "o"
    },
    {
      name: "ServiceResolverNew",
      inSignature: "iisssiu",
      outSignature: "o"
    },
    {
      name: "HostNameResolverNew",
      inSignature: "iisiu",
      outSignature: "o"
    },
    {
      name: "AddressResolverNew",
      inSignature: "iisu",
      outSignature: "o"
    },
    {
      name: "RecordBrowserNew",
      inSignature: "iisqqu",
      outSignature: "o"
    }
  ],
  signals: [
    {
      name: "StateChanged",
      inSignature: "is"
    }
  ]
};


function AvahiServer() {
    this._init();
}

AvahiServer.prototype = {
    _init: function() {
        DBus.system.proxifyObject(this, 'org.freedesktop.Avahi', '/');
    }
};
DBus.proxifyPrototype(AvahiServer.prototype, AvahiServerIface);


/*
 * Avahi ServiceBrowser object
 */

var ServiceBrowserIface = {
  name: "org.freedesktop.Avahi.ServiceBrowser",
  methods: [
    {
      name: "Free",
      inSignature: "",
      outSignature: ""
    }
  ],
  signals: [
    {
      name: "ItemNew",
      inSignature: "iisssu"
    },
    {
      name: "ItemRemove",
      inSignature: "iisssu"
    },
    {
      name: "Failure",
      inSignature: "s"
    },
    {
      name: "AllForNow",
      inSignature: ""
    },
    {
      name: "CacheExhausted",
      inSignature: ""
    }
  ]
};

function AvahiServiceBrowser(objpath) {
    this._init(objpath);
}

AvahiServiceBrowser.prototype = {
    _init: function(objpath) {
        DBus.system.proxifyObject(this, 'org.freedesktop.Avahi', objpath);
    }
};
DBus.proxifyPrototype(AvahiServiceBrowser.prototype, ServiceBrowserIface);


/*
 * The test
 */

let avahi = new AvahiServer();
let browser = null;

function on_item_new(proxy, iface, protocol, name, stype, domain, flags) {
    print("NEW");
    print(name);
}

function on_item_remove(proxy, iface, protocol, name, stype, domain, flags) {
    print("REMOVE");
    print(name);
}

function on_failure(message) {
    print("FAILURE");
    print(message);
}

function on_all_for_now() {
    print("ALL FOR NOW");
    Mainloop.quit("iphonemon");
}

function on_cache_exhausted() {
    print("CACHE EXHAUSTED");
}

//"_link411._udp"

path = avahi.ServiceBrowserNewRemoteSync(-1, -1, "_http._tcp", "", 0);
print("Got path " + path);
browser = new AvahiServiceBrowser(path);
browser.connect("ItemNew", on_item_new);
browser.connect("ItemRemove", on_item_remove);
browser.connect("Failure", on_failure);
browser.connect("AllForNow", on_all_for_now)
browser.connect("CacheExhausted", on_cache_exhausted);
Mainloop.run('iphonemon');

/*
avahi.ServiceBrowserNewRemote(-1, -1, "_http._tcp", "", 0, function(result, error) {
    if (error) {
        log(error);
        Mainloop.quit("iphonemon");
    } else {
        print("Got path " + result);
        browser = new AvahiServiceBrowser(result);
        browser.connect("ItemNew", on_item_new);
        browser.connect("ItemRemove", on_item_remove);
        browser.connect("Failure", on_failure);
        browser.connect("AllForNow", on_all_for_now)
        browser.connect("CacheExhausted", on_cache_exhausted);
    }
});
Mainloop.run('iphonemon');
*/