# AGENTS.md

## Project

C99 CLI utility. Reads command definitions from `/opt/cmd/`, presents a numbered menu, executes the chosen command. Chinese UI.

Single source file: `cmd.c` (176 lines).

## Usage

- `cmd` — show `/opt/cmd/readme` and list command files
- `cmd <name>` — parse `/opt/cmd/<name>`, show numbered menu, prompt for a choice
- `cmd <name> <n>` — execute menu item `n` directly (skips the prompt; non-numeric or out-of-range values are rejected)

## Build

Two options:

- **Makefile:** `make` (plain gcc, output: `cmd`)
- **CMake:** `mkdir build && cd build && cmake .. && make` (output: `build/cmd`)

## Install

- `make install` copies `cmd` to `/usr/local/bin/cmd` and sets permissions on `/opt/cmd/sh/*.*`
- Requires `/opt/cmd/` to exist at runtime with command definition files and `readme`

## Runtime paths

- Command files: `/opt/cmd/<name>` (format: 3 lines per command — description, flag, command string)
- Readme: `/opt/cmd/readme`
- Installed binary: `/usr/local/bin/cmd`

## Conventions

- All user-facing strings are Chinese (error messages, prompts)
- C99 standard
- No tests, no linter, no CI configured
- Single commit repo — keep changes minimal and focused
