#!/usr/bin/env bash

export all_ok=0

#shellcheck source=common
. scripts/common

check_instruction "insert 0 hello world" "basic-write-file" 1 1 1

check_output_of_instruction_is_correct "hello world" "insert 0 hello world" "basic-write-file" 1

new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "012abc3456789" "insert 3 abc" "basic-write-file" 0 "$new_file"

check_instruction "insert 0 \x01\x02\x03" "basic-write-file" 1 1 1

check_output_of_instruction_is_correct "   " "insert 0 \x20\x20\x20" "basic-write-file" 1

new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "0123 456789" "insert 4 \x20" "basic-write-file" 0 "$new_file"

check_output_of_instruction_is_correct "a b c" "insert 0 a\x20b\x20c" "basic-write-file" 1

echo "$all_ok"
