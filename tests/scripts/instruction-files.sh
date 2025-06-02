#!/usr/bin/env bash

export all_ok=0

test_title="instruction-files"

#shellcheck source=common
. scripts/common

check_output_of_instruction_file_is_correct "hello world" "insert 0 these contents will be removed
remove 3 29; 0 1; 0 0


insert 0 o; 0 l; 0 l; 0 e; 0 h
insert 5  ; 6 watla



replace 10 d; 7 orl


" "$test_title" 1

echo "$all_ok"
