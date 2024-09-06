#!/usr/bin/env bash

export all_ok=0

#shellcheck source=common
. scripts/common

new_file="$(create_test_file "0123456789")"
check_reader_command "basic-read-file" "$new_file" 1

new_file="$(create_test_file "0123456789")"
check_reader_output_is_correct "0123456789" "basic-read-file" "$new_file"

new_file="$(create_test_file "0123456789")"
check_reader_output_is_correct "012" "basic-read-file" "$new_file" 0 0 0 0 0 0 3

new_file="$(create_test_file "0123456789")"
check_reader_output_is_correct "789" "basic-read-file" "$new_file" 0 0 0 0 0 7

new_file="$(create_test_file "0123456789")"
check_reader_output_is_correct "456" "basic-read-file" "$new_file" 0 0 0 0 0 4 3

echo "$all_ok"
