#!/usr/bin/env bash

polonius-editor tmp/new-file -a "insert 0 hello world" >/dev/null 2>&1

if [[ $? -eq 0 ]] && [[ -f tmp/new-file ]] && [[ "$(cat tmp/new-file)" == "hello world" ]]; then
	echo "0"
	rm -f tmp/new-file
	exit
fi

rm -f tmp/new-file

echo "1"
