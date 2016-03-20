#!/bin/bash

SRCDIR=html_src
BLDDIR=html

rm -rf "$BLDDIR/pages"
rm -rf "$BLDDIR/js"
rm -rf "$BLDDIR/css"

cd "$SRCDIR"
gulp
cd ..

cp -R "$SRCDIR/css" "$BLDDIR"
cp -R "$SRCDIR/images" "$BLDDIR"
cp -R "$SRCDIR/js" "$BLDDIR"

find "$BLDDIR" -name "*.map" -delete

php "$SRCDIR/home.php" > "$BLDDIR/home.tpl"
php "$SRCDIR/wifi.php" > "$BLDDIR/wifi.tpl"
