#!/usr/bin/env bash

export all_ok=0

test_title="refuse-invalid-instructions"

#shellcheck source=common
. scripts/common

check_instruction "insert 1 invalid" "$test_title" 0 0 1
check_instruction "insert 0 valid; 25 invalid" "$test_title" 0 0 1
check_instruction "replace 0 invalid" "$test_title" 0 0 1
check_instruction "remove 0 0" "$test_title" 0 0 1
check_instruction "remove -1 -2" "$test_title" 0 0 1
check_instruction "not a valid instruction code" "$test_title" 0 0 1

new_file=$(create_test_file "0123456789")
check_instruction "insert 12 invalid" "$test_title" 0 1 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_instruction "insert 0 valid; 100 invalid" "$test_title" 0 1 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_instruction "replace 10 a" "$test_title" 0 1 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_instruction "replace end 01234567890" "$test_title" 0 1 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_instruction "remove 0 a" "$test_title" 0 1 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_instruction "remove a 0" "$test_title" 0 1 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_instruction "remove 11 11" "$test_title" 0 1 0 "$new_file"

echo "$all_ok"
