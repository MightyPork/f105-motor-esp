#!/bin/bash

xterm -title "ESP html build" -e "source $HOME/.bashrc && cd html_src && gulp watch"
