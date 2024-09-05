#!/usr/bin/env bash

export all_ok=0

#shellcheck source=common
. scripts/common

check_instruction "insert 1 invalid" "refuse-invalid-instructions" 0 0 1
check_instruction "replace 0 invalid" "refuse-invalid-instructions" 0 0 1
check_instruction "remove 0 0" "refuse-invalid-instructions" 0 0 1
check_instruction "remove -1 -2" "refuse-invalid-instructions" 0 0 1
check_instruction "not a valid instruction code" "refuse-invalid-instructions" 0 0 1

echo "$all_ok"
