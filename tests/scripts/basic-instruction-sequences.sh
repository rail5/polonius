#!/usr/bin/env bash

export all_ok=0

test_title="basic-instruction-sequences"

#shellcheck source=common
. scripts/common

check_output_of_instruction_is_correct "hello world" "insert 0 hello ; 6 world" "$test_title" 1

check_output_of_instruction_is_correct "hello world" "insert 0 hello; 5  ; 6 world" "$test_title" 1

check_output_of_instruction_is_correct "abc" "insert 0 c; 0 b; 0 a" "$test_title" 1

new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "0123456789a c" "insert 10 a; 11 \x20; 12 c" "$test_title" 0 "$new_file"


new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "0123456" "remove 9 9; 8 8; 7 7" "$test_title" 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "0246789" "remove 1 1; 2 2; 3 3" "$test_title" 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "34789" "remove 0 2; 2 3" "$test_title" 0 "$new_file"


new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "abc3456def" "replace 0 abc; 7 def" "$test_title" 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "d f3456789" "replace 0 abc; 0 d\x20f" "$test_title" 0 "$new_file"


check_output_of_instruction_sequence_is_correct "hello world" "insert 0 these contents will be removed
remove 3 29; 0 1; 0 0
insert 0 o; 0 l; 0 l; 0 e; 0 h
insert 5  ; 6 watla
replace 10 d; 7 orl

" "$test_title" 1

echo "$all_ok"
