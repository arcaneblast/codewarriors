#!/bin/bash

if [ -f "$1" ]
then
	exit 0
else
	exit 1
fi
