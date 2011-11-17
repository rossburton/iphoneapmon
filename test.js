#! /usr/bin/gjs

/*
 * Copyright (C) 2011 Intel Corp
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option) any
 * later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this package; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

const Ap = imports.gi.Ap;
const Mainloop = imports.mainloop;

let ap = new Ap.Monitor();

ap.connect("found", function (monitor, name) {
    print ("Found an AP called " + name);
});


ap.connect("lost", function (monitor, name) {
    print ("Lost an AP called " + name);
});

ap.connect("update", function (monitor, name, strength, tech) {
    print ("Got a " + Ap.Monitor.tech_to_string (tech) + " connection at " + strength + "% on " + name);
});

Mainloop.run('ap');
