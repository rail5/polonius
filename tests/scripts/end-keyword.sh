#!/usr/bin/env bash

export all_ok=0

#shellcheck source=common
. scripts/common

# INSERT 'end'
check_instruction "insert end this is a new file" "end-keywords" 1 1 1

new_file=$(create_test_file "0123456789")
check_instruction "insert end this is an old file" "end-keywords" 1 1 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "0123456789abc" "insert end abc" "end-keywords" 0 "$new_file"

# REPLACE 'end'
new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "012345678a" "replace end a" "end-keywords" 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "9876543210" "replace end 9876543210" "end-keywords" 0 "$new_file"

# REMOVE 'end'
new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "012345678" "remove end end" "end-keywords" 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "" "remove 0 end" "end-keywords" 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "01234" "remove 5 end" "end-keywords" 0 "$new_file"

echo "$all_ok"
