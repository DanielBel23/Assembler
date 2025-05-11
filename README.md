# Assembler Project

Welcome to my **first ever programming project** – an educational assembler built in C!

It translates a **fictional assembly language** designed for a **15-bit processor** into binary machine code.

---

## Project Overview

The assembler takes `.as` source files written in a simplified assembly language and converts them into machine-readable binary output. It handles labels, directives, and instructions in a structured two-pass process to generate correct and validated machine code.

---

## Assembler Rules

- Input files use a fictional **assembly language** tailored for a **15-bit processor architecture**.
- The assembler:
  - Parses and validates **labels**, **directives**, and **instructions**.
  - Reports **syntax errors** and **illegal usage** clearly.
  - Produces up to three output files: `.ob` (object), `.ent` (entry points), and `.ext` (external references).

### Supported Directives
- `.data` — followed by integers.
- `.string` — followed by a quoted string.
- `.extern` — declares external labels.
- `.entry` — marks labels for external visibility in output.

### Labels
- Must start with a letter and consist of alphanumeric characters.
- Maximum length: typically 31 characters.
- Cannot use reserved names or conflict with instruction/register names.

### Instructions & Addressing
- Supported addressing modes:
  - Immediate (e.g. `#5`)
  - Direct (`LABEL`)
  - Structured (`LABEL[r1]`)
  - Register direct (`r2`)
- Only valid combinations of operands and addressing modes are accepted.

### Two-Pass Compilation
1. **First Pass**: Collect labels, validate syntax, build the symbol table, and determine memory layout.
2. **Second Pass**: Resolve symbols and generate final binary code.

### Error Handling
- The assembler identifies and reports:
  - Unknown commands.
  - Operand count mismatches.
  - Invalid addressing modes.
  - Undefined or duplicate labels.

---

## Output Files

For each valid input file:
- `.ob`: Memory address + binary encoding.
- `.ent`: Entry label declarations.
- `.ext`: External symbol usages.

---

## How to Run

1. make sure the files you want to use as an input are in the root directory (not in inputAndOutput)
2. Compile the project using:
   ```
   make
   ```
3. Run on an input file:
   ```
   ./assembler input.as
   ```
