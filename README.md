# SafePLC-Mini

> Mini soft-PLC en C++17 avec safety functions, conçu comme projet de

[![CI](https://img.shields.io/badge/CI-ready-brightgreen)](.github/workflows/ci.yml)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue)](CMakeLists.txt)
[![Coverage](https://img.shields.io/badge/coverage-target%20%E2%89%A595%25-success)](tools/coverage.sh)
[![License](https://img.shields.io/badge/license-MIT-lightgrey)](LICENSE)

## Ce que ce projet démontre

|--------------------------|--------------|
| Développement C++ sur projets complexes | Architecture en couches, RAII, smart usage, FSM |
| Tests unitaires (objectif 100%) | GoogleTest, fixtures, fakes, tous chemins erreur testés |
| Analyse statique (Coverity / Parasoft) | clang-tidy (subset MISRA-C++/CERT) + cppcheck, configurés en CI |
| Code coverage | gcov + lcov + seuil 95% bloquant dans la CI |
| Normes safety strictes | Patterns IEC 61508 (watchdog), ISO 13850 (E-Stop), ISO 13851 (Two-Hand), voteurs 1oo2/2oo3 |
| Industrialisation | CMake presets, GitHub Actions matrix, ASan/UBSan, clang-format |
| Linux | Build & run natif Ubuntu 22.04 |

## Modules livrés

| Module | Fichier | Norme/Référence | Coverage cible |
|--------|---------|------------------|----------------|
| Voter 1-out-of-2 | `core/include/safeplc/safety/voter_1oo2.hpp` | IEC 61508 redundancy | 100% |
| Voter 2-out-of-3 | `core/include/safeplc/safety/voter_2oo3.hpp` | IEC 61508 redundancy | 100% |
| Emergency Stop (latching) | `core/include/safeplc/safety/estop.hpp` | EN ISO 13850 | 100% |
| Two-Hand Control | `core/include/safeplc/safety/two_hand.hpp` | EN ISO 13851 (IIIC) | 100% |
| Software Watchdog | `core/include/safeplc/safety/watchdog.hpp` | IEC 61508 diag | 100% |
| Safety FSM | `core/include/safeplc/fsm/safety_fsm.hpp` | — | 100% |
| Diagnostic ring log | `core/include/safeplc/diag/diag_log.hpp` | — | 100% |

## Structure du repo

```
safeplc-mini/
├── core/                       # Bibliothèque safety pure (header-only, testable host)
│   └── include/safeplc/
│       ├── ports/i_clock.hpp       # Interface horloge (mockable)
│       ├── safety/                 # voter, estop, two_hand, watchdog
│       ├── fsm/safety_fsm.hpp      # State machine top-level
│       └── diag/diag_log.hpp       # Ring buffer diagnostique
├── app/                        # Daemon Linux (utilise core + LinuxClock)
├── test/                       # GoogleTest — 1 fichier par module + fakes/
├── cmake/                      # Helpers (warnings, coverage)
├── tools/                      # Scripts coverage / clang-tidy / cppcheck
├── .github/workflows/ci.yml    # CI : build matrix + sanitizers + coverage + static analysis
├── .clang-format               # Style Google + 100 colonnes
├── .clang-tidy                 # Subset MISRA-C++ / CERT
├── CMakeLists.txt
├── CMakePresets.json
├── README.md                   # (ce fichier)
└── SETUP.md                    # Guide pas à pas pour un junior
```

## Démarrage rapide

```bash
# Pré-requis : Ubuntu 22.04+ avec build-essential, cmake, ninja, lcov, clang-tidy, cppcheck
sudo apt install build-essential cmake ninja-build lcov clang-tidy cppcheck

# 1) Build + tests
cmake --preset=debug
cmake --build --preset=debug
ctest --preset=debug

# 2) Coverage HTML
bash tools/coverage.sh
xdg-open build/coverage/html/index.html

# 3) Static analysis
bash tools/run_clang_tidy.sh
bash tools/run_cppcheck.sh

# 4) Démo daemon
./build/debug/app/safeplc_app
```

**Procédure détaillée pour un ingénieur junior :** voir [SETUP.md](SETUP.md).

## Architecture safety en bref

Toute la logique est **pure** (ne touche pas le HW). Les dépendances temps /
I/O sont injectées via interfaces (`IClock`), ce qui rend chaque module
testable à 100% sans `sleep()` et sans hardware réel.

```
┌────────────────────────────────────────────────┐
│  app/main.cpp  (daemon Linux)                  │
│     ┌──────────────┐  ┌──────────────┐         │
│     │ LinuxClock   │  │ (futurs HAL) │         │
│     └──────┬───────┘  └──────────────┘         │
└────────────┼───────────────────────────────────┘
             │ injection
┌────────────▼───────────────────────────────────┐
│  core/  (pure logic — header-only — testable)  │
│     SafetyFsm  +  Voter1oo2 / Voter2oo3        │
│     EStop      +  TwoHandControl  +  Watchdog  │
│     DiagLog                                    │
└────────────────────────────────────────────────┘
             ▲ injection
┌────────────┴───────────────────────────────────┐
│  test/  (GoogleTest + FakeClock)               │
│     1 fichier de test par module — 100% L+B    │
└────────────────────────────────────────────────┘
```

## Cibles qualité

| Métrique | Cible | Mesurée par |
|----------|-------|-------------|
| Couverture lignes (core) | ≥ 95% | `lcov` (bloquant CI) |
| Couverture branches (core) | ≥ 90% | `lcov --rc lcov_branch_coverage=1` |
| Warnings compilateur | 0 (`-Werror`) | GCC + Clang × Debug + Release |
| clang-tidy | 0 (`WarningsAsErrors: '*'`) | Subset MISRA-C++ + CERT |
| cppcheck | 0 (`--error-exitcode=1`) | Niveau warning + style + perf |
| ASan + UBSan | 0 | Préset `asan` |

## À pitcher en entretien

> *« J'ai construit un mini soft-PLC en C++17 sur Linux qui implémente une
> bibliothèque de safety functions inspirée des normes IEC 61508 et
> ISO 13850/13851 — voteurs redondants, E-Stop avec verrouillage anti-redémarrage,
> two-hand control, software watchdog. Toute la logique est découplée du HW via
> interfaces injectables, ce qui me permet 100% de couverture sur le cœur avec
> GoogleTest, et un seuil bloquant à 95% dans la CI. La CI lance également
> clang-tidy en mode MISRA-C++/CERT (équivalent de ce que ferait Coverity ou
> Parasoft), cppcheck, et AddressSanitizer pour couvrir les UB runtime. »*

## Roadmap suggérée

| Phase | Ajout | Effort |
|-------|-------|--------|
| v0.2 | Adapter Modbus TCP (libmodbus) | 6-8h |
| v0.3 | Cycle scan déterministe + measure jitter | 4-6h |
| v0.4 | Pre-commit hook + Doxygen + badge coverage | 3h |
| v0.5 | Fuzzing libFuzzer sur diag log + FSM | 4-6h |

## Licence

MIT — voir [LICENSE](LICENSE).
