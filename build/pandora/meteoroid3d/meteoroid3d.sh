#!/bin/sh
export PATH=":${PATH:-"/usr/bin:/bin:/usr/local/bin"}"
export LD_LIBRARY_PATH=":${LD_LIBRARY_PATH:-"/usr/lib:/lib"}"
export HOME="/mnt/utmp/meteoroid3d" XDG_CONFIG_HOME="/mnt/utmp/meteoroid3d"

if [ -d /mnt/utmp/meteoroid3d/share ];then
	export XDG_DATA_DIRS=/mnt/utmp/meteoroid3d/share:$XDG_DATA_DIRS:/usr/share
fi
export SDL_AUDIODRIVER="alsa"

cat /proc/pandora/nub0/mode > /tmp/nub0mode_before_meteoroid3d
cat /proc/pandora/nub1/mode > /tmp/nub1mode_before_meteoroid3d
echo absolute > /proc/pandora/nub0/mode
echo absolute > /proc/pandora/nub1/mode

LD_LIBRARY_PATH=. ./meteoroid3d $*

cat /tmp/nub0mode_before_meteoroid3d > /proc/pandora/nub0/mode
cat /tmp/nub1mode_before_meteoroid3d > /proc/pandora/nub1/mode
 
rm /tmp/nub0mode_before_meteoroid3d /tmp/nub1mode_before_meteoroid3d
