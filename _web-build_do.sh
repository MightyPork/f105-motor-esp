#!/bin/bash

set -e
set -x

SRCDIR=html_src
BLDDIR=html

rm -rf "$BLDDIR/pages"
rm -rf "$BLDDIR/js"
rm -rf "$BLDDIR/css"

cd "$SRCDIR"
gulp --production
cd ..

cp -R "$SRCDIR/css" "$BLDDIR"
cp -R "$SRCDIR/js" "$BLDDIR"

find "$BLDDIR" -name "*.map" -delete

mkdir -p "$BLDDIR/pages"

php "$SRCDIR/page_status.php" > "$BLDDIR/pages/status.tpl"
php "$SRCDIR/page_wifi.php" > "$BLDDIR/pages/wifi.tpl"
php "$SRCDIR/page_waveform.php" > "$BLDDIR/pages/waveform.html" # no substitutions, .html allows to gzip it.
