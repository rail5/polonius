#!/usr/bin/env bash

export all_ok=0

#shellcheck source=common
. scripts/common

check_output_of_instruction_is_correct "hello world" "insert 0 hello ; 6 world" "basic-instruction-sequences" 1

check_output_of_instruction_is_correct "hello world" "insert 0 hello; 5  ; 6 world" "basic-instruction-sequences" 1

check_output_of_instruction_is_correct "abc" "insert 0 c; 0 b; 0 a" "basic-instruction-sequences" 1

new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "0123456789a c" "insert end a; end \x20; end c" "basic-instruction-sequences" 0 "$new_file"


new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "0123456" "remove end end; end end; end end" "basic-instruction-sequences" 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "0246789" "remove 1 1; 2 2; 3 3" "basic-instruction-sequences" 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "34789" "remove 0 2; 2 3" "basic-instruction-sequences" 0 "$new_file"


new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "abc3456def" "replace 0 abc; end def" "basic-instruction-sequences" 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "abc3456def" "replace 0 abc; end def" "basic-instruction-sequences" 0 "$new_file"

new_file=$(create_test_file "0123456789")
check_output_of_instruction_is_correct "d f3456789" "replace 0 abc; 0 d\x20f" "basic-instruction-sequences" 0 "$new_file"


check_output_of_instruction_set_is_correct "hello world" "insert 0 these contents will be removed
remove 3 end; 0 1; 0 end
insert 0 o; 0 l; 0 l; 0 e; 0 h
insert end  ; 6 watla
replace end d; 7 orl" "basic-instruction-sequences" 1

echo "$all_ok"
