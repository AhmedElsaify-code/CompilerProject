
# Mini C Scanner

A simple C++ lexical analyzer for a subset of the C language,
built using C++17 and std::regex.

## Supported Features

- Main function keyword
- Variable declarations and identifiers
- Integer numbers
- While loops
- Relational and arithmetic operators
- Braces, parentheses, commas and semicolons
- Single-line and multi-line comments
- Lexical error reporting with line and column numbers

## Project Files

- `token.h`: Shared Token structure
- `scanner.h`: Scanner interface
- `scanner.cpp`: Lexical analysis implementation
- `main.cpp`: Program entry point and output
- `input.c`: Sample C input

## Requirements

- A C++17 compiler, such as g++
- Git (optional, for version control)

## Build

```bash
g++ -std=c++17 -Wall -Wextra main.cpp scanner.cpp -o scanner.exe
```

## Run

```bash
./scanner.exe input.c
```

On Windows PowerShell, use:

```powershell
.\scanner.exe input.c
```

## Parser Integration

Include `scanner.h` and use the shared `Token` structure.

Call:

```cpp
std::vector<Token> tokens;
std::vector<LexicalError> errors;

bool success = scanSource(source, tokens, errors);
```

If `success` is true, the tokens are ready for the parser.
The parser should use each token's type, value, line and column.

This project performs lexical analysis only. It does not
validate the full syntax or semantics of C.
