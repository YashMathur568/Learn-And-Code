# Coding Guidelines

Welcome to the coding guidelines for this project. These guidelines are designed to ensure that our code is consistent, readable, maintainable, and efficient. Please follow these principles when contributing to the codebase.

## Naming Conventions

- Use `camelCase` for variable names.
- Use `PascalCase` for classes, structs, and enums.
- Choose meaningful names that clearly indicate purpose.

## Functions and Methods

- Prefer a maximum of 2-3 arguments per function. If more are needed, consider using a struct or class.
- Keep functions to no more than 20-25 lines, following the Single Responsibility Principle.
- Validate function inputs and parameters.

## Code Quality Principles

- Follow the Do Not Repeat Yourself (DRY) Principle.
- Prioritize readability over conciseness.
- Write code that is easy to test.
- Refactor code when it becomes difficult to understand.

## Error Handling and Safety

- Properly handle errors and exceptions.
- Ensure proper resource cleanup.
- Avoid global variables whenever possible.

## Constants and Magic Numbers

- Use `UPPERCASE` for constants.
- Avoid magic numbers; use named constants or enums.
- Use enums for related groups of constants.

## Class Design

- Keep classes small and focused on a single responsibility.
- Prefer composition over inheritance when possible.
- Avoid exposing internal class data directly.
- Separate interface and implementation clearly.

## Best Practices

- Keep code formatted and indented properly.
- Delete unnecessary code rather than commenting it out.
- Prefer const correctness wherever possible.
- Pass large objects by const reference instead of by value.
- Avoid unnecessary dependencies between modules.
- Keep header files minimal and avoid unnecessary includes.
- Ensure code compiles without warnings.
- Keep logical blocks separated with whitespace.
- Prefer modular design over large monolithic code.
- Avoid hardcoding configuration values.

By following these guidelines, we can maintain a high-quality codebase that is easy to understand, modify, and extend.