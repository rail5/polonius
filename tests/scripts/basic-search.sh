#!/usr/bin/env bash

export all_ok=0

test_title="basic-search"

#shellcheck source=common
. scripts/common

new_file="$(create_test_file "0123456789")"
check_reader_output_is_correct "3 5" "$test_title" "$new_file" 1 "345" 0 0 1

new_file="$(create_test_file "0123456789")"
check_reader_command "$test_title" "$new_file" 0 1 "999"

new_file="$(create_test_file "0123456789")"
check_reader_command "$test_title" "$new_file" 0 1 "567" 0 0 0 0 3

new_file="$(create_test_file "a b")"
check_reader_command "$test_title" "$new_file" 1 1 "\x20" 0 1

new_file="$(create_test_file "a b")"
check_reader_output_is_correct " " "$test_title" "$new_file" 1 "\x20" 0 1

new_file="$(create_test_file "a b")"
check_reader_output_is_correct "1 1" "$test_title" "$new_file" 1 "\x20" 0 1 1

echo "$all_ok"
