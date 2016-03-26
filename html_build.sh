#!/bin/bash

set -e
set -x

SRCDIR=html_src
BLDDIR=html

rm -rf "$BLDDIR/pages"
rm -rf "$BLDDIR/js"
rm -rf "$BLDDIR/css"

cd "$SRCDIR"
gulp
cd ..

cp -R "$SRCDIR/css" "$BLDDIR"
cp -R "$SRCDIR/js" "$BLDDIR"

find "$BLDDIR" -name "*.map" -delete

mkdir -p "$BLDDIR/pages"

php "$SRCDIR/home.php" > "$BLDDIR/pages/home.tpl.html"
php "$SRCDIR/wifi.php" > "$BLDDIR/pages/wifi.tpl.html"
