#!/usr/bin/env bash

all_ok=0

# Testing editor
polonius-editor tmp/new-file -a "insert 0 hello world" >/dev/null 2>&1
editor_first_return_code=$?
editor_first_file_exists=$([[ -f tmp/new-file ]])
editor_first_file_contents=$(cat tmp/new-file)

rm -f tmp/new-file

polonius-editor "tmp/another file" -a "insert 0 hello world" >/dev/null 2>&1
editor_second_return_code=$?
editor_second_file_exists=$([[ -f "tmp/another file" ]])
editor_second_file_contents=$(cat "tmp/another file")

rm -f "tmp/another file"

if [[ $editor_second_return_code -eq $editor_first_return_code ]] && [[ $editor_second_file_exists -eq $editor_first_file_exists ]] && [[ "$editor_second_file_contents" == "$editor_first_file_contents" ]]; then
	# Everything OK
	true
else
	all_ok=1 # Everything not OK
	{
		echo "--"
		echo "POLONIUS-EDITOR DEBUG INFO"
		echo "First return code: $editor_first_return_code"
		echo "First file exists: $editor_first_file_exists"
		echo "First file contents: $editor_first_file_contents"
		echo "Second return code: $editor_second_return_code"
		echo "Second file exists: $editor_second_file_exists"
		echo "Second file contents: $editor_second_file_contents"
	}  >> debug/doing-the-same-thing-twice-gets-the-same-result
fi


# Testing reader
echo "hello world" > tmp/new-file
echo "hello world" > "tmp/another file"

polonius-reader tmp/new-file >/dev/null 2>&1
reader_first_return_code=$?
reader_first_file_contents=$(polonius-reader tmp/new-file)

polonius-reader "tmp/another file" >/dev/null 2>&1
reader_second_return_code=$?
reader_second_file_contents=$(polonius-reader "tmp/another file")

rm -f tmp/new-file
rm -f "tmp/another file"

if [[ $reader_first_return_code -eq $reader_second_return_code ]] && [[ "$reader_first_file_contents" == "$reader_second_file_contents" ]]; then
	# Everything OK
	true
else
	all_ok=1 # Everything not OK
	{
		echo "--"
		echo "POLONIUS-READER DEBUG INFO"
		echo "First return code: $reader_first_return_code"
		echo "First file contents: $reader_first_file_contents"
		echo "Second return code: $reader_second_return_code"
		echo "Second file contents: $reader_second_file_contents"
	}  >> debug/doing-the-same-thing-twice-gets-the-same-result
fi

echo "$all_ok"
