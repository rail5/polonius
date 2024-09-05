#!/usr/bin/env bash

echo "hello world" > tmp/new-file

polonius-reader tmp/new-file >/dev/null 2>&1 && can_continue=true

if [[ "$can_continue" == "true" ]] && [[ "$(polonius-reader tmp/new-file)" == "hello world" ]]; then
	echo "0"
	rm -f tmp/new-file
	exit
fi

rm -f tmp/new-file

echo "1"
