#!/usr/bin/env bash
# Generate an HTML coverage report from the coverage build.
# Usage:  ./tools/coverage.sh
# Output: build/coverage/html/index.html
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${REPO_ROOT}/build/coverage"

cd "${REPO_ROOT}"

# 1) Configure + build with coverage flags.
cmake --preset=coverage
cmake --build --preset=coverage --parallel

# 2) Run the tests (this generates the .gcda files).
ctest --preset=coverage

# 3) Capture coverage data.
lcov --capture --directory "${BUILD_DIR}" --output-file "${BUILD_DIR}/coverage.info" \
     --rc lcov_branch_coverage=1 --ignore-errors gcov,source,graph

# 4) Strip out third-party + test code.
lcov --remove "${BUILD_DIR}/coverage.info" \
     '/usr/*' \
     '*/_deps/*' \
     '*/test/*' \
     '*/app/*' \
     --output-file "${BUILD_DIR}/coverage.filtered.info" \
     --rc lcov_branch_coverage=1 --ignore-errors gcov,source,graph

# 5) Render HTML.
genhtml "${BUILD_DIR}/coverage.filtered.info" \
        --output-directory "${BUILD_DIR}/html" \
        --branch-coverage --rc genhtml_branch_coverage=1

echo
echo "Coverage report: file://${BUILD_DIR}/html/index.html"

# 6) Print a one-line summary (parsable by CI).
lcov --summary "${BUILD_DIR}/coverage.filtered.info" --rc lcov_branch_coverage=1
