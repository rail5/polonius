#!/usr/bin/env bash

all_ok=0

function check_instruction() {
	local instruction="$1" file_should_exist="$2" new_file="$3" file_name="$4"

	if [[ "$file_name" == "" ]]; then
		file_name="new-file"
	fi

	if [[ "$new_file" == "1" ]]; then
		rm -f "tmp/${file_name:?}"
	fi

	polonius-editor "tmp/$file_name" -a "$instruction" >/dev/null 2>&1
	return_code=$?
	file_exists=$([[ -f "tmp/$file_name" ]] && echo 1 || echo 0)

	#rm -f "tmp/$file_name"

	if [[ $return_code -ne 0 ]] && [[ $file_exists -eq $file_should_exist ]]; then
		# Everything OK
		true
	else
		all_ok=1 # Everything not OK
		{
			echo "--"
			echo "INSTRUCTION: $instruction"
			echo "Return code: $return_code"
			echo "File exists?: $file_exists"
			echo "File should exist?: $file_should_exist"
			echo "File name: tmp/$file_name"
		}  >> debug/refuse-invalid-instructions
	fi
}

check_instruction "insert 1 invalid" 0 1
check_instruction "replace 0 invalid" 0 1
check_instruction "remove 0 0" 0 1
check_instruction "remove -1 -2" 0 1
check_instruction "not a valid instruction code" 0 1

echo "$all_ok"
