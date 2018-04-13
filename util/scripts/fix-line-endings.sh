#!/bin/bash

[ -z $1 ] && echo "File not found" && exit 1

tr -d $'\r' < "$1" > /tmp/tmpfile
cp /tmp/tmpfile $1