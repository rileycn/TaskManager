# Code Standard for TaskManager

This document defines the code style, structure, and best practices to be followed when contributing to TaskManager

---

## General Guidelines

1. **Code Structure**:
   - Organize the code into logical modules (e.g., `processes_panel.c`, `misc.c`).
   - Place reusable functions in separate files, e.g., miscellaneous functions in `misc.h/c`.

2. **Naming Conventions**:
   - **Files**: Use lowercase with underscores (e.g., `processes_panel.c`, `resources_graph.h`).
   - **Functions**: Use `snake_case` for regular functions and prefix with module name (e.g., `processes_parse`).
   - **Constants**: Use `SCREAMING_SNAKE_CASE`.

3. **Formatting**:
   - Curly braces `{}`:
     - Place the opening brace on the same line as the declaration.
     - Use braces even for single-line conditional statements.
   - Use one blank line between function definitions and logically separate blocks.

---

## Header Files  

1. **Header Guards**:
   - Use include guards for all header files:
     ```c
     #ifndef PROCESSES_PANEL_H
     #define PROCESSES_PANEL_H
     ...
     #endif
     ```

2. **Includes**:
   - Include system headers before project headers.
   - Use angle brackets (`<>`) for system headers and quotes (`""`) for project headers.

---

## Variables  

1. **Declaration**:
   - Declare variables at the top of a function or in the smallest applicable scope.
   - Always initialize variables where applicable.

---

## Memory Management  

1. **Dynamic Allocation**:
   - Free all dynamically allocated memory.

2. **File Handles**:
   - Close all opened file descriptors explicitly with `fclose` or `closedir`.

---

End of document.
