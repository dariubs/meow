# meow

Minimal programming language for `.mw` source files. REPL and script runner.

## Install

### Linux

- Install clang and readline dev, e.g.:
  - Debian/Ubuntu: `sudo apt install clang libreadline-dev`
  - Fedora: `sudo dnf install clang readline-devel`

### Mac

- Install **Xcode Command Line Tools** (for clang): `xcode-select --install`
- Optional (REPL line editing and history): `brew install readline`

### Windows

- Install **LLVM** (clang): [llvm.org/downloads](https://releases.llvm.org/download.html) or `winget install LLVM.LLVM`
- Add clang to your PATH. No readline needed; the REPL uses a simple line input.



## Build

If you see `'stdio.h' file not found` on Mac, install Xcode Command Line Tools: `xcode-select --install`. The Makefile uses the macOS SDK so clang can find system headers. If you see a dyld error about `libz3` or "Abort trap: 6", the Makefile uses the system clang (`/usr/bin/clang`) on Mac so it does not pull in Homebrew's LLVM and its z3 dependency.

**Mac / Linux** (with make):

```bash
git clone https://github.com/dariubs/meow.git && cd meow
make
```

**Windows** (no make): from the project directory in Command Prompt or PowerShell:

```bash
clang -DMEOW_NO_READLINE -o meow.exe main.c repl.c run.c lexer.c parser.c interp.c
```

On Windows the binary is `meow.exe`. On Mac/Linux it is `meow`.

## Run

- **REPL** (interactive): `./meow` (Mac/Linux) or `meow.exe` (Windows)
- **Script**: `./meow script.mw` or `meow.exe script.mw`

Type `exit` in the REPL to quit.

## Syntax

- **Numbers**: `42`, `3.14`
- **Variables**: `name = expr;` (e.g. `x = 1;`)
- **Print**: `print expr;` (e.g. `print 1 + 2;`)
- **If**: `if (expr) stmt` and optional `else stmt` (single statement each)
- **While**: `while (expr) stmt` (single statement)
- **Expressions**: `+ - * /`, comparisons `== != < > <= >=`, parentheses, unary `-`
- **Comments**: `#` to end of line

Statements end with `;`. No blocks `{}` in this version; use one statement after `if`/`while`.

### Example script (`example.mw`)

```mw
# minimal meow script
x = 10;
print x;
print 1 + 2 * 3;
if (x > 5) print 42;
else print 0;
while (x > 0) x = x - 1;
print x;
```

Run: `./meow example.mw` (output: `10`, `7`, `42`, then `0` after the loop).

### REPL example

```
(meow)> print 1 + 1;
2
(meow)> a = 3;
(meow)> print a * 2;
6
(meow)> exit
```
