#! /usr/bin/gjs

const Ap = imports.gi.Ap;
const Mainloop = imports.mainloop;

let ap = new Ap.Monitor();
ap.connect("update", function (monitor, strength, tech) {
    print ("Got a " + Ap.Monitor.tech_to_string (tech) + " connection at " + strength + "%");
});
Mainloop.run('ap');
