#!/bin/sh
PROGRAM="meteoroid3d"
VERSION="1.0.3"
DEST=./build/*
echo "<html>" > index.htm
echo "<head>" >> index.htm
echo "</head>" >> index.htm
echo "<body>" >> index.htm
TIME=`date -u +"%d.%m.%Y %R"`
echo "Updated at the $TIME." >> index.htm
echo "<h1>$PROGRAM download links:</h1>" >> index.htm
echo "<?php" > symlink.php
for f in $DEST
do
	if [ -e "$f/$PROGRAM/$PROGRAM" ] || [ -e "$f/$PROGRAM/$PROGRAM.exe" ]; then
		NAME=`echo "$f" | cut -d/ -f3 | cut -d. -f1`
		echo "$NAME:"
		echo "--> Copy temporary folders"
		cp -r data "$f/$PROGRAM"
		cd $f
		echo "--> Create archive"
		if [ $NAME = "pandora" ]; then
			cd $PROGRAM
			../make_package.sh
			cd ..
			echo "<a href=$PROGRAM.pnd>$NAME</a></br>" >> ../../index.htm
		else
			if [ $NAME = "i386" ] || [ $NAME = "amd64" ]; then
				tar cfvz "$PROGRAM-$NAME-$VERSION.tar.gz" * > /dev/null
				mv "$PROGRAM-$NAME-$VERSION.tar.gz" ../..
				echo "<a href=$PROGRAM-$NAME-$VERSION.tar.gz>$NAME</a></br>" >> ../../index.htm
				echo "unlink('$PROGRAM-$NAME.tar.gz');" >> ../../symlink.php
				echo "symlink('$PROGRAM-$NAME-$VERSION.tar.gz', '$PROGRAM-$NAME.tar.gz');" >> ../../symlink.php
			else
				#no Pandora, no PC. So we have a low resolution device!
				cp "../../Play-Bold.ttf" "$PROGRAM/data"
				if [ $NAME = "gcw" ]; then
					mksquashfs * "$PROGRAM.opk" -all-root -noappend -no-exports -no-xattrs
					mv "$PROGRAM.opk" ../..
					echo "<a href=$PROGRAM.opk type=\"application/x-opk+squashfs\">$NAME</a></br>" >> ../../index.htm
				else
					zip -r "$PROGRAM-$NAME-$VERSION.zip" * > /dev/null
					mv "$PROGRAM-$NAME-$VERSION.zip" ../..
					echo "<a href=$PROGRAM-$NAME-$VERSION.zip>$NAME</a></br>" >> ../../index.htm
					echo "unlink('$PROGRAM-$NAME.zip');" >> ../../symlink.php
					echo "symlink('$PROGRAM-$NAME-$VERSION.zip', '$PROGRAM-$NAME.zip');" >> ../../symlink.php
				fi
			fi
		fi
		echo "--> Remove temporary folders"
		rm -r $PROGRAM/data
		cd ..
		cd ..
	fi
done
echo "?>" >> symlink.php
echo "</body>" >> index.htm
echo "</html>" >> index.htm
