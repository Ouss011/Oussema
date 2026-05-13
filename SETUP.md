# SETUP.md — Guide pas à pas pour ingénieur junior

> **Objectif :** partir d'une machine Linux propre et arriver à un build vert
> avec tests, couverture 95%+, et analyse statique passante en **moins de 30 minutes**.

Ce guide suppose **zéro connaissance préalable** de CMake, GoogleTest, lcov ou
clang-tidy. Chaque commande peut être copiée-collée. Toute erreur fréquente est
listée en fin de section avec sa solution.

---

## Table des matières

1. [Pré-requis système](#1-pré-requis-système)
2. [Cloner le repo](#2-cloner-le-repo)
3. [Build & tests (premier passage)](#3-build--tests-premier-passage)
4. [Lire la couverture de code](#4-lire-la-couverture-de-code)
5. [Lancer l'analyse statique](#5-lancer-lanalyse-statique)
6. [Lancer le daemon de démonstration](#6-lancer-le-daemon-de-démonstration)
7. [Travailler en boucle courte (edit / build / test)](#7-travailler-en-boucle-courte)
8. [Ajouter ton propre module + tests](#8-ajouter-ton-propre-module--tests)
9. [Configurer VS Code](#9-configurer-vs-code-recommandé)
10. [Dépannage — erreurs fréquentes](#10-dépannage--erreurs-fréquentes)
11. [Glossaire](#11-glossaire)

---

## 1. Pré-requis système

### 1.1 OS supporté

| OS | Statut |
|----|--------|
| Ubuntu 22.04 / 24.04 LTS | ✅ Officiel |
| Debian 12+ | ✅ |
| WSL2 (Ubuntu) sur Windows 10/11 | ✅ |
| macOS | ⚠️ Build OK, coverage via `gcov` peut nécessiter `gcc` Homebrew |
| Windows natif | ❌ Non supporté (scripts Bash) |

### 1.2 Installer les paquets

Sur Ubuntu/Debian, **une seule commande** :

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    lcov \
    clang \
    clang-tidy \
    clang-format \
    cppcheck
```

### 1.3 Vérifier les versions minimales

```bash
cmake --version       # ≥ 3.20
ninja --version       # ≥ 1.10
g++ --version         # ≥ 9 (C++17 complet)
clang++ --version     # ≥ 11
lcov --version        # ≥ 1.14
```

Si l'une est trop ancienne, voir [§10 Dépannage](#10-dépannage--erreurs-fréquentes).

---

## 2. Cloner le repo

```bash
git clone <url-du-repo> safeplc-mini
cd safeplc-mini
```

Si tu ne l'as pas encore sur Git :

```bash
cd /chemin/vers/safeplc-mini
git init
git add .
git commit -m "Initial commit"
```

---

## 3. Build & tests (premier passage)

### 3.1 Configurer (génère les fichiers de build)

```bash
cmake --preset=debug
```

**Ce que ça fait :** CMake lit `CMakeLists.txt`, télécharge GoogleTest
(première fois seulement, ~30 secondes), et écrit les fichiers de build dans
`build/debug/`.

Tu dois voir en bas du log :

```
===== safeplc-mini configuration =====
  C++ standard       : 17
  Build type         : Debug
  Build tests        : ON
  ...
======================================
```

### 3.2 Compiler

```bash
cmake --build --preset=debug --parallel
```

**Ce que ça fait :** compile tout le code source et les tests, en parallèle
sur tous les cores disponibles. Durée typique : 20-60 secondes au premier
build, 2-5 secondes ensuite (cache).

### 3.3 Lancer les tests

```bash
ctest --preset=debug
```

Tu dois voir :

```
Test project /home/.../safeplc-mini/build/debug
      Start  1: VoterOneOoTwoTest.BothLowReturnsLowOk
 1/N  Test  #1: VoterOneOoTwoTest.BothLowReturnsLowOk ........... Passed 0.00 sec
      ...
 N/N  Test #N: DiagLogTest.ClearEmptiesBuffer ................... Passed 0.00 sec

100% tests passed, 0 tests failed out of N
```

**Si un test échoue :** voir [§10 Dépannage](#10-dépannage--erreurs-fréquentes).

---

## 4. Lire la couverture de code

### 4.1 Générer le rapport HTML

```bash
bash tools/coverage.sh
```

**Ce que ça fait :** rebuild en mode coverage (instrumentation `--coverage`),
relance les tests, agrège les fichiers `.gcda` avec `lcov`, filtre les
dépendances tierces, et génère un site HTML dans `build/coverage/html/`.

Durée : 30 secondes à 1 minute.

### 4.2 Ouvrir le rapport

```bash
# Sur Linux
xdg-open build/coverage/html/index.html
# Sur WSL2
explorer.exe build/coverage/html/index.html
# Sur macOS
open build/coverage/html/index.html
```

Tu verras un dashboard avec :
- **Lines coverage** : pourcentage de lignes exécutées (cible ≥ 95%).
- **Functions coverage** : fonctions appelées au moins une fois.
- **Branches coverage** : branches `if/else/switch` couvertes (cible ≥ 90%).

Clique sur un fichier pour voir ligne par ligne. Code vert = couvert, rouge =
non couvert.

### 4.3 Comprendre le sumamry CLI

À la fin du script, tu vois :

```
Summary coverage rate:
  lines......: 98.7% (152 of 154 lines)
  functions..: 100.0% (28 of 28 functions)
  branches...: 95.3% (61 of 64 branches)
```

Si `lines < 95%`, la CI échouera. Trouve les lignes rouges et ajoute des tests.

---

## 5. Lancer l'analyse statique

### 5.1 clang-tidy (équivalent Coverity / Parasoft)

```bash
bash tools/run_clang_tidy.sh
```

**Ce que ça fait :** parcourt tous les fichiers `core/` et `app/`, applique
les règles définies dans `.clang-tidy` (subset MISRA-C++ / CERT), et fait
échouer si une seule règle est violée.

Durée : 10-30 secondes.

**Résultat attendu :** `clang-tidy: OK` en fin de sortie.

### 5.2 cppcheck

```bash
bash tools/run_cppcheck.sh
```

**Ce que ça fait :** détecte les bugs courants (use-after-free, null deref,
shadowing, leak, etc.). Complémentaire à clang-tidy.

**Résultat attendu :** `cppcheck: OK`.

---

## 6. Lancer le daemon de démonstration

```bash
./build/debug/app/safeplc_app
```

Tu verras :

```
safeplc-mini starting — Ctrl-C to stop
[t=12345ms] state=Run v=1 wd_ok=1
[t=13345ms] state=Run v=1 wd_ok=1
...
```

`Ctrl-C` pour stopper.

**Ce que ça démontre :** le daemon orchestre `Voter2oo3 + EStop + Watchdog +
SafetyFsm`. Toutes les 5 secondes, un glitch est injecté sur le canal C — la
FSM passe en Fault quand le voter détecte une discrepancy persistante.

---

## 7. Travailler en boucle courte

Une fois le premier passage fait, ta boucle de dev devient :

```bash
# Modifier un fichier dans core/ ou test/

# Rebuild (incrémentale, ~2-5 s)
cmake --build --preset=debug --parallel

# Rerun tests (~1 s)
ctest --preset=debug --output-on-failure
```

### 7.1 Lancer un seul test

```bash
./build/debug/test/test_voter_2oo3
```

Ou avec filtre :

```bash
./build/debug/test/test_voter_2oo3 --gtest_filter='*Discrepancy*'
```

### 7.2 Lancer les tests avec sanitizers (ASan + UBSan)

```bash
cmake --preset=asan
cmake --build --preset=asan --parallel
ctest --preset=asan
```

Tu obtiens un crash explicite si un test cause un undefined behavior ou un
out-of-bounds.

---

## 8. Ajouter ton propre module + tests

Exemple : ajouter un module `core/include/safeplc/safety/light_curtain.hpp`.

### 8.1 Créer le header

```bash
mkdir -p core/include/safeplc/safety
$EDITOR core/include/safeplc/safety/light_curtain.hpp
```

Contenu minimal :

```cpp
#pragma once
namespace safeplc::safety {
class LightCurtain {
public:
    bool update(bool beam_broken) noexcept { triggered_ = beam_broken; return triggered_; }
    [[nodiscard]] bool triggered() const noexcept { return triggered_; }
private:
    bool triggered_ = false;
};
}  // namespace safeplc::safety
```

### 8.2 Créer le test

```bash
$EDITOR test/test_light_curtain.cpp
```

```cpp
#include "safeplc/safety/light_curtain.hpp"
#include <gtest/gtest.h>

TEST(LightCurtain, NotTriggeredInitially) {
    safeplc::safety::LightCurtain lc;
    EXPECT_FALSE(lc.triggered());
}

TEST(LightCurtain, TriggeredWhenBeamBroken) {
    safeplc::safety::LightCurtain lc;
    EXPECT_TRUE(lc.update(true));
}
```

### 8.3 Déclarer le test dans CMake

Édite `test/CMakeLists.txt` et ajoute :

```cmake
safeplc_add_test(test_light_curtain)
```

### 8.4 Rebuild + test

```bash
cmake --build --preset=debug --parallel
ctest --preset=debug
```

---

## 9. Configurer VS Code (recommandé)

### 9.1 Extensions

| Extension | Identifier | Rôle |
|-----------|------------|------|
| CMake Tools | `ms-vscode.cmake-tools` | Build presets dans la sidebar |
| C/C++ | `ms-vscode.cpptools` | IntelliSense |
| clangd | `llvm-vs-code-extensions.vscode-clangd` | LSP rapide (préférer à C/C++ IntelliSense) |
| GitLens | `eamodio.gitlens` | Blame inline |

### 9.2 Settings recommandés

`.vscode/settings.json` :

```json
{
    "cmake.configureOnOpen": true,
    "cmake.useCMakePresets": "always",
    "C_Cpp.intelliSenseEngine": "disabled",
    "clangd.arguments": [
        "--compile-commands-dir=build/debug",
        "--clang-tidy",
        "--header-insertion=never"
    ],
    "editor.formatOnSave": true,
    "[cpp]": {
        "editor.defaultFormatter": "llvm-vs-code-extensions.vscode-clangd"
    }
}
```

Pas besoin de `c_cpp_properties.json` — clangd lit `compile_commands.json`
automatiquement (généré par CMake dans `build/debug/`).

---

## 10. Dépannage — erreurs fréquentes

### "CMake Error: Could not find CMAKE_CXX_COMPILER"

Tu n'as pas installé `build-essential` ou `g++`. Refais §1.2.

### "Could not find ninja"

Installe-le : `sudo apt install ninja-build`. Ou retire `"generator": "Ninja"`
des presets pour utiliser Make.

### "FetchContent download failed"

Tu n'as pas accès Internet ou GitHub est bloqué. Soit utilise un proxy
(`HTTPS_PROXY=...`), soit télécharge manuellement GoogleTest 1.14 et
remplace `URL` par `SOURCE_DIR` pointant vers ton extraction.

### "Test failed: undefined behavior in ..."

Lance avec sanitizers :

```bash
cmake --preset=asan && cmake --build --preset=asan && ctest --preset=asan
```

Le message ASan/UBSan te donne la stack trace exacte.

### "lcov: ERROR: unused/inconsistent..."

Versions de lcov anciennes. Solution :

```bash
sudo apt remove lcov
sudo apt install lcov  # Ubuntu 22.04+ a lcov 1.15
# Ou installer depuis sources :
git clone https://github.com/linux-test-project/lcov.git
cd lcov && sudo make install
```

### "clang-tidy: error: unknown warning option '-Wno-...'"

Versions de clang-tidy < 14 ne connaissent pas certains checks récents.
Installe : `sudo apt install clang-tidy-15` puis `update-alternatives`.

### "CMake version 3.16 is too old"

Ubuntu 20.04 a CMake 3.16. Installe une version récente :

```bash
wget -qO - https://apt.kitware.com/keys/kitware-archive-latest.asc | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'
sudo apt update && sudo apt install cmake
```

### "Coverage script: lcov branch coverage not enabled"

Édite `~/.lcovrc` :

```
lcov_branch_coverage = 1
genhtml_branch_coverage = 1
```

---

## 11. Glossaire

| Terme | Définition courte |
|-------|-------------------|
| **AAA pattern** | Arrange / Act / Assert — structure d'un test unitaire |
| **ASan** | AddressSanitizer — détecte buffer overflows, use-after-free runtime |
| **CMake preset** | Configuration nommée dans `CMakePresets.json` |
| **Coverage (line)** | % de lignes exécutées au moins une fois par les tests |
| **Coverage (branch)** | % de branches `if/else/switch` empruntées par les tests |
| **CTest** | Lanceur de tests intégré à CMake |
| **Fake** | Implémentation simplifiée d'une dépendance pour les tests |
| **FetchContent** | Mécanisme CMake pour télécharger une dépendance à la configuration |
| **Fixture** | Classe GoogleTest qui partage du setup entre plusieurs tests |
| **GoogleTest (gtest)** | Framework de tests unitaires C++ |
| **GMock** | Extension de GoogleTest pour mocker des interfaces |
| **gcov** | Outil GCC qui mesure la couverture de code |
| **HAL** | Hardware Abstraction Layer — couche qui isole le HW |
| **IEC 61508** | Norme générale safety pour systèmes électroniques |
| **lcov** | Frontend pour gcov, génère des rapports HTML |
| **MISRA C++** | Guide de codage safety pour C++ (très répandu automotive/industrie) |
| **Mock** | Test double qui vérifie des interactions |
| **Ninja** | Build system rapide (alternative à Make) |
| **RAII** | Resource Acquisition Is Initialization — ressources liées au scope |
| **Stub** | Test double qui retourne des valeurs fixées |
| **UBSan** | UndefinedBehaviorSanitizer — détecte les UB runtime |

---

**Tu es prêt.** Commande de bonne foi pour vérifier que tout est en place :

```bash
cmake --preset=debug && \
cmake --build --preset=debug --parallel && \
ctest --preset=debug && \
bash tools/coverage.sh && \
bash tools/run_clang_tidy.sh && \
bash tools/run_cppcheck.sh && \
echo "All green — ready to pitch."
```
