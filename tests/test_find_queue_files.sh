#!/bin/bash
# Verify find_queue_files emits the right set of paths per layout.
set -uo pipefail

cd "$(dirname "$0")/.."
source tests/lib.sh
source ./afl-cov

trap 'rm -rf "$TMP"' EXIT
TMP=$(mktmp)

# AFL single: 2 queue files, crashes/timeouts excluded
AFL_DIR="$TMP/afl-single"; mkfixture_afl_single "$AFL_DIR"
FUZZER_LAYOUT="afl"
find_queue_files | assert_count 2 "afl-single queue count"

# AFL parallel: 3 workers * 2 queue files = 6
AFL_DIR="$TMP/afl-parallel"; mkfixture_afl_parallel "$AFL_DIR"
FUZZER_LAYOUT="afl"
find_queue_files | assert_count 6 "afl-parallel queue count"

# libFuzzer flat: 2 corpus files, 5 artifacts excluded
AFL_DIR="$TMP/libfuzzer"; mkfixture_libfuzzer "$AFL_DIR"
FUZZER_LAYOUT="flat"
find_queue_files | assert_count 2 "libfuzzer queue count"

# honggfuzz flat: 2 corpus files, 2 SIG*.fuzz + REPORT excluded
AFL_DIR="$TMP/honggfuzz"; mkfixture_honggfuzz "$AFL_DIR"
FUZZER_LAYOUT="flat"
find_queue_files | assert_count 2 "honggfuzz queue count"

echo "[PASS] find_queue_files"
