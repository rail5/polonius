#!/usr/bin/env bash

if [[ "$(which polonius-editor)" == "$(realpath ../bin/polonius-editor)" ]] && [[ "$(which polonius-reader)" == "$(realpath ../bin/polonius-reader)" ]]; then
	echo "0"
	exit
fi

echo "1"
# Output debug information to the relevant debug file if we failed
{
	echo "Path is set to:" 
	echo "$PATH"
	echo "We are running $(which polonius-editor) instead of ../bin/polonius-editor"
	echo "we are running $(which polonius-reader) instead of ../bin/polonius-reader"
} > debug/are-tests-working
