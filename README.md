# OOP
# Lab 1: Logging Library & Parentheses Validator

**Student:** Frunza Adrian  
**Course:** Object-Oriented Programming (Lab 1)  
**Instructor:** Dominic Flocea  
**Repository:**  https://github.com/adrianfza/OOP

---

## Overview

This directory contains the C++17 implementation for Laboratory 1:
1. **`Logger`**: Configurable logging class demonstrating static members (`level`, `format`) and instance members (`name`). Supports severity suppression, custom format placeholders (`%t`, `%l`, `%n`, `%m`), and extra-credit contextual messages (`%r`).
2. **`ParenthesesValidator`**: Syntax validator using object composition with `Logger` to evaluate balanced bracket sequences via an $O(n)$ stack-based scan.

## Contents

- `main.cpp`: Complete C++17 implementation and automated test suite.
- `REPORT.md`: Academic laboratory report answering all Section 7.1 questions.
- `Laboratory_Report_1_Frunza_Adrian.pdf`: Formatted 2-page academic report document.

## Build and Run

Compile and run using any standard C++17 compiler (`clang++` or `g++`):

```bash
# Compile
clang++ -std=c++17 -Wall -Wextra main.cpp -o lab1

# Run tests
./lab1
```

## Test Coverage

- **Core test cases**:
  - `()` &rarr; `true`
  - `)(()())` &rarr; `false`
  - `(` &rarr; `false`
  - `(())((()())())` &rarr; `true`
- **Edge cases**: Empty string `""`, deeply nested `((()))`, unbalanced opens `(()`, premature closes `())`, inverted pair `)(`.
- **Level suppression**: Verifies that setting `level = INFO` hides `TRACE`, and `level = ERROR` hides `INFO`/`WARN`.
- **Custom format templates**: Tests formatting strings with different placeholder combinations.
- **Extra credit `%r`**: Tests dynamic injection of random level-associated status messages.
