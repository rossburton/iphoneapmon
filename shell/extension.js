/* -*- mode: js2; js2-basic-offset: 4; indent-tabs-mode: nil -*- */

const DBus = imports.dbus;
const Gdk = imports.gi.Gdk;
const GLib = imports.gi.GLib;
const Lang = imports.lang;
const Shell = imports.gi.Shell;
const St = imports.gi.St;

const Main = imports.ui.main;
const PanelMenu = imports.ui.panelMenu;
const PopupMenu = imports.ui.popupMenu;
const Panel = imports.ui.panel;

const Ap = imports.gi.Ap;

function Indicator() {
    this._init.apply(this, arguments);
}

Indicator.prototype = {
    __proto__: PanelMenu.SystemStatusButton.prototype,

    _init: function() {
        PanelMenu.SystemStatusButton.prototype._init.call(this, 'network-idle', null);

        this.actor.hide();

        this._ap = new Ap.Monitor()
        this._ap.connect("found", Lang.bind(this, this._on_found));
        this._ap.connect("update", Lang.bind(this, this._on_update));
        this._ap.connect("lost", Lang.bind(this, this._on_lost));

        this._item = new PopupMenu.PopupMenuItem("foo", { reactive: false });
        this.menu.addMenuItem(this._item);
    },

    _on_found: function (monitor, name) {
        this._item.label.text = name;
        this.actor.show();
    },

    _on_update: function (monitor, name, strength, tech) {
        this._item.label.text = "" +
            Ap.Monitor.tech_to_string (tech) +
            " connection at " + strength + "% on " + name;
        if (strength == 0) {
            this.setIcon("network-wireless-signal-none");
        } else if (strength <= 20) {
            this.setIcon("network-wireless-signal-weak");
        } else if (strength <= 60) {
            this.setIcon("network-wireless-signal-ok");
        } else if (strength <= 80) {
            this.setIcon("network-wireless-signal-good");
        } else if (strength <= 100) {
            this.setIcon("network-wireless-signal-excellent");
        }
    },

    _on_lost: function (monitor, name) {
        this.actor.hide();
    }
}


function init(metadata) {
    // TODO: i18n
}

let _indicator;

function enable() {
    _indicator = new Indicator();
    Main.panel.addToStatusArea('iphoneapmon', _indicator);
}

function disable() {
    _indicator.destroy();
}
