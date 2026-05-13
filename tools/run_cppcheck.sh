#!/usr/bin/env bash
# Static analysis with cppcheck — complements clang-tidy.
# Usage: ./tools/run_cppcheck.sh
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "${REPO_ROOT}"

cppcheck \
    --enable=warning,style,performance,portability \
    --inconclusive \
    --error-exitcode=1 \
    --suppress=missingIncludeSystem \
    --suppress=unusedFunction \
    --std=c++17 \
    --quiet \
    -I core/include \
    core/ app/

echo "cppcheck: OK"
