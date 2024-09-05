#!/usr/bin/env bash

if [[ "$(which polonius-editor)" == "$(realpath ../polonius-editor)" ]] && [[ "$(which polonius-reader)" == "$(realpath ../polonius-reader)" ]]; then
	echo "0"
	exit
fi

echo "1"
# Output debug information to the relevant debug file if we failed
echo "Path is set to:" > debug/are-tests-working
echo "$PATH" >> debug/are-tests-working