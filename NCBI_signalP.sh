#!/bin/bash

in_signalp=$1
awk '/^lcl/ {print $1}' $in_signalp > $2
