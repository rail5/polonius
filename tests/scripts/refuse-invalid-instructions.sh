#!/usr/bin/env bash

export all_ok=0

#shellcheck source=common
. scripts/common

check_instruction "insert 1 invalid" "refuse-invalid-instructions" 0 0 1
check_instruction "replace 0 invalid" "refuse-invalid-instructions" 0 0 1
check_instruction "remove 0 0" "refuse-invalid-instructions" 0 0 1
check_instruction "remove -1 -2" "refuse-invalid-instructions" 0 0 1
check_instruction "not a valid instruction code" "refuse-invalid-instructions" 0 0 1

new_file=$(create_test_file "0123456789")
check_instruction "insert 11 invalid" "refuse-invalid-instructions" 0 1 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_instruction "replace 10 a" "refuse-invalid-instructions" 0 1 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_instruction "replace end 01234567890" "refuse-invalid-instructions" 0 1 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_instruction "remove 0 a" "refuse-invalid-instructions" 0 1 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_instruction "remove a 0" "refuse-invalid-instructions" 0 1 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_instruction "remove 10 10" "refuse-invalid-instructions" 0 1 0 "$new_file"

echo "$all_ok"
