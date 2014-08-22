/* -*- mode: js2; js2-basic-offset: 4; indent-tabs-mode: nil -*- */

const Clutter = imports.gi.Clutter;
const Gdk = imports.gi.Gdk;
const GLib = imports.gi.GLib;
const Lang = imports.lang;
const Shell = imports.gi.Shell;
const St = imports.gi.St;

const Main = imports.ui.main;
const PanelMenu = imports.ui.panelMenu;

const Ap = imports.gi.Ap;

function _addParentBin(actor) {
    let parentActor = actor.get_parent();

    // Create the bin next to the actor
    let bin = new Clutter.Actor();
    parentActor.insert_child_above(bin, actor);

    // Move the actor from the parent to the bin
    actor.ref();
    parentActor.remove_child(actor);
    bin.add_child(actor);
    actor.unref();

    // And return the bin
    return bin;
}

function _removeParentBin(actor) {
    let parentBin = actor.get_parent();
    let parentActor = parentBin.get_parent();

    actor.ref();
    parentBin.remove_child(actor);
    parentActor.insert_child_above(parentBin, actor);
    actor.unref();

    parentBin.destroy();
}

const Indicator = new Lang.Class({
    Name: 'iPhoneAP',
    Extends: PanelMenu.SystemIndicator,

    _init: function() {
        this.parent();

        this._signal_icon = this._addIndicator();
        this._signal_icon.icon_name = 'network-idle-symbolic';

        this._tech_icon = this._addIndicator();
        this._tech_icon.icon_name = 'network-cellular-connected-symbolic';

        this._signal_icon.visible = this._tech_icon.visible = false;

        this._ap = new Ap.Monitor();
        this._ap.connect("found", Lang.bind(this, this._on_found));
        this._ap.connect("update", Lang.bind(this, this._on_update));
        this._ap.connect("lost", Lang.bind(this, this._on_lost));
    },

    _on_found: function (monitor, name) {
        this._signal_icon.icon_name = 'network-idle-symbolic';
        this._tech_icon.icon_name = 'network-cellular-connected-symbolic';
        this._signal_icon.visible = this._tech_icon.visible = true;

        this._networkBin.visible = false;
    },

    _on_update: function (monitor, name, strength, tech) {
        this._signal_icon.visible = true;
        this._signal_icon.gicon = Ap.Monitor.get_icon_for_signal (strength);

        let icon = null;
        if (strength != 0)
            icon = Ap.Monitor.get_icon_for_tech (tech);
        this._tech_icon.visible = (icon != null);

        this._tech_icon.gicon = icon;
        this._tech_icon.visible = true;
    },

    _on_lost: function (monitor, name) {
        this._networkBin.visible = true;
        this._signal_icon.visible = this._tech_icon.visible = false;
    }
});

const Extension = new Lang.Class({
    Name: 'SystemMonitor.Extension',

    _init: function() {
    },

    enable: function() {
        this._indicator = new (Indicator);
        this._agMenu = Main.panel.statusArea['aggregateMenu'];

        // Put our icon left of the network indicator
        this._agMenu._indicators.insert_child_above(this._indicator.indicators,
                                                    this._agMenu._network.indicators);

        // Put the primary network indicator into its own bin
        this._indicator._networkBin = _addParentBin(this._agMenu._network._primaryIndicator);

        this.visible = false;
    },

    disable: function() {
        // Put the primary network indicator back in its original position
        _removeParentBin(this._indicator._networkBin);

        this._indicator.indicators.destroy();
    },

});

function init() {
    return new Extension();
}
