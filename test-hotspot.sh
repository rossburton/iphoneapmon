#!/bin/sh

STRENGTH0= 
STRENGTH1=
STRENGTH2=
STRENGTH3=
STRENGTH4=
STRENGTH5=

avahi-publish-service "Hotspot.local" _link411._udp 666 TXT ss=$STRENGTH0 di=3G
