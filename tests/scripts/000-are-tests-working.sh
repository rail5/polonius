#!/usr/bin/env bash

echo "Path is set to:" > debug/are-tests-working
echo "$PATH" >> debug/are-tests-working

if [[ "$(which polonius-editor)" == "$(realpath ../polonius-editor)" ]] && [[ "$(which polonius-reader)" == "$(realpath ../polonius-reader)" ]]; then
	echo "0"
	exit
fi

echo "1"
