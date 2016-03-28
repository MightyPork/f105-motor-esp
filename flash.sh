#!/bin/bash

xterm -title "ESP flash" -e "source $HOME/.bashrc && make flash -B -j4"
