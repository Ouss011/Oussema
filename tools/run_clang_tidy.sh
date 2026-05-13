#!/usr/bin/env bash
# Run clang-tidy with the project's .clang-tidy config (subset of MISRA/CERT).
# Requires a configured build dir (compile_commands.json).
# Usage: ./tools/run_clang_tidy.sh
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${REPO_ROOT}/build/debug"

if [[ ! -f "${BUILD_DIR}/compile_commands.json" ]]; then
    echo "compile_commands.json not found in ${BUILD_DIR}"
    echo "Run:   cmake --preset=debug   first"
    exit 1
fi

# Prefer run-clang-tidy if present (parallel), fall back to plain clang-tidy.
if command -v run-clang-tidy >/dev/null 2>&1; then
    run-clang-tidy \
        -p "${BUILD_DIR}" \
        -header-filter='.*/(core|app)/.*' \
        -quiet \
        "${REPO_ROOT}/core/" "${REPO_ROOT}/app/"
else
    find "${REPO_ROOT}/core" "${REPO_ROOT}/app" \
        \( -name '*.cpp' -o -name '*.hpp' \) -print0 |
        xargs -0 -n1 clang-tidy -p "${BUILD_DIR}"
fi

echo "clang-tidy: OK"
