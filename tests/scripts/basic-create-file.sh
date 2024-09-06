#!/usr/bin/env bash

export all_ok=0

#shellcheck source=common
. scripts/common

check_instruction "insert 0 hello world" "basic-create-file" 1 1 1

check_output_of_instruction_is_correct "hello world" "insert 0 hello world" "basic-create-file" 1

echo "$all_ok"
