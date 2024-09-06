#!/usr/bin/env bash

export all_ok=0

test_title="basic-regex-search"

#shellcheck source=common
. scripts/common

new_file="$(create_test_file "0123456789")"
check_reader_command "$test_title" "$new_file" 1 1 "[0-9]+" 1

new_file="$(create_test_file "0123456789")"
check_reader_output_is_correct "012" "$test_title" "$new_file" 1 "[0-9]{3}" 1

new_file="$(create_test_file "here is some random 123-456-7890 data.")"
check_reader_output_is_correct "123-456-7890" "$test_title" "$new_file" 1 "([0-9]{3})-([0-9]{3})-([0-9]{4})" 1

new_file="$(create_test_file "here is some random 123-456-7890 data.")"
check_reader_output_is_correct "123-456-7890" "$test_title" "$new_file" 1 "[0-9][^a-z ]+" 1

new_file="$(create_test_file "here is some random 123-456-7890 data.")"
check_reader_output_is_correct "12" "$test_title" "$new_file" 1 "[0-9]{1,2}" 1

new_file="$(create_test_file "here is some random 123-456-7890 data.")"
check_reader_output_is_correct "data" "$test_title" "$new_file" 1 "[a-z]+" 1 0 0 19

new_file="$(create_test_file "here is some random 123-456-7890 data.")"
check_reader_output_is_correct "data." "$test_title" "$new_file" 1 "[a-z]+\." 1

echo "$all_ok"
