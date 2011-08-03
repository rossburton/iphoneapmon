#! /usr/bin/env gjs

const Avahi = imports.gi.Avahi;
const AvahiCore = imports.gi.AvahiCore;
const Mainloop = imports.mainloop;

let client = new Avahi.Client();

function start_browser(client) {
    print ("starting browser");
    let browser = Avahi.ServiceBrowser.new("_link411._udp");
    browser.connect("new-service", function (browser, iface, proto, name, type, domain, flags) {
        print ("found " + name);
        let resolver = Avahi.ServiceResolver.new(iface, proto, name, type, domain, -1, 0);
        resolver.attach(client);

        resolver.connect("found", function(resolver, iface, protocol, name, type, domain, host, address, port, txt, flags) {
            print ("resolved " + name);
            print (txt);
        });
    });
    browser.connect("removed-service", function (browser, iface, proto, name, type, domain, flags) {
        print ("lost " + name);
    });
    browser.connect("cache-exhausted", function () {
        print ("cache exhausted");
    });
    browser.connect("all-for-now", function () {
        print ("all for now");
    });

    browser.attach(client);
}

client.connect("state-changed", function (client, state) {
    if (state == Avahi.ClientState.GA_CLIENT_STATE_S_RUNNING) {
        print("running");
        start_browser(client);
    }
});
client.start();


Mainloop.run('iphonemon');
