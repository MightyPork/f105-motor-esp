#!/bin/bash

set -e
set -x

SRCDIR=html_src
BLDDIR=html

rm -rf "$BLDDIR/pages"
rm -rf "$BLDDIR/js"
rm -rf "$BLDDIR/css"
rm -rf "$BLDDIR/img"

cd "$SRCDIR"
gulp --production
cd ..

cp -R "$SRCDIR/css" "$BLDDIR"
cp -R "$SRCDIR/js" "$BLDDIR"
cp -R "$SRCDIR/img" "$BLDDIR"

find "$BLDDIR" -name "*.map" -delete

mkdir -p "$BLDDIR/pages"

php "$SRCDIR/page_home.php" > "$BLDDIR/pages/home.tpl"
php "$SRCDIR/page_wifi.php" > "$BLDDIR/pages/wifi.tpl"
