
Jumbo Shell(jsh) - A Simple Unix-like Shell

Description:

Jumbo Shell(jsh) is a basic Unix-like shell implemented in C. It provides a command-line interface for users to enter commands. The shell supports both built-in commands and the execution of external programs. For external programs, jsh can execute commands with pipes (|) to create pipelines, enabling the output of one command to be used as the input for another. The shell also handles errors gracefully, displaying appropriate error messages for command not found and file not found situations.

Features:

- Basic command-line interface.
- Support for executing external programs.
- Built-in "exit" command to terminate the shell.
- Pipeline support using the "|" symbol.
- Error handling for command not found and file not found errors.
- Concurrent execution of commands in pipelines.

Usage:

- Compile the shell.c source code.
- Run the compiled program to start the shell.
- Enter commands, including pipelines, at the shell prompt.

Notes:

All lines from the shell start with "jsh" followed by the "$" symbol as the prompt.
The shell handles pipelines by concurrently executing commands in the pipeline.
Error messages are displayed for command not found and file not found errors.
Proper handling of child processes to prevent zombie processes.
