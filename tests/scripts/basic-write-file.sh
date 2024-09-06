#!/usr/bin/env bash

export all_ok=0

#shellcheck source=common
. scripts/common

check_instruction "insert 0 hello world" "basic-write-file" 1 1 1

check_output_of_instruction_is_correct "hello world" "insert 0 hello world" "basic-write-file" 1

new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "012abc3456789" "insert 3 abc" "basic-write-file" 0 "$new_file"

echo "$all_ok"
