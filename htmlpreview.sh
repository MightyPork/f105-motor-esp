#!/bin/bash

rm -rf html_preview

cp -rs "$PWD/html" "$PWD/html_preview/"

for file in $(find html_preview/ -name "*.tpl")
do 
  mv $file `echo $file | sed s/.tpl$/.html/`
done
