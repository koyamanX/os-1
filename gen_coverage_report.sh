#!/bin/bash

find . -type f -name "*.gcda" | xargs -I@ gcov -b @ 
lcov --rc lcov_branch_coverage=1 -c -d . -o tmp.info
lcov --rc lcov_branch_coverage=1 -b -c -d . -r tmp.info  "$(pwd)/tests/*" -o output.info
genhtml --branch-coverage -o coverage_report -p . -f output.info

rm -rf output.info tmp.info *.gcov
