iphoneapmon
===========

Monitor the iPhone connection state when using the Personal Hotspot feature on iOS 7 and earlier.

This repository contains:
 - a C library with gobject-introspection support
 - in the wip-gnome-3-14-extension branch, an updated, and unfinished extension that should work with GNOME Shell 3.14
 - a test application to replicate the output of the iPhone
 - a test application to monitor the state of the iPhone
 
Note that, as this feature on iOS 8 and later does not use mDNS to export the state of the connection, this code is now considered abandoned.
