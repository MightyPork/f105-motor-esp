#!/bin/bash

xterm -title "ESP html build" -e "source $HOME/.bashrc && make flash -B -j4"
