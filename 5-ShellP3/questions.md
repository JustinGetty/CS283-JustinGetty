1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

The implementation calls wait or waitpid on every forked child process (or uses a loop to reap them) before it returns to the prompt. This ensures that the shell blocks until every child process in the pipeline has finished. If waitpid() were not called on all children, they might continue running in the background (or become zombie processes), leading to resource leaks and potential interference with subsequent commands

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

After dup2() is used to redirect file descriptors, the original pipe ends remain open. It is necessary to close these unused ends to free system resources and to signal EOF properly; if they remain open, the reading process might hang waiting for more data, since the pipe would not be closed, causing deadlocks or unintended behavior

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

The cd command is implemented as a built-in because it must change the current working directory of the shell process itself. If cd were an external command executed in a separate child process, the directory change would only affect that child process, leaving the parent shell’s directory unchanged and rendering cd ineffective.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To support an arbitrary number of piped commands, the shell could use dynamic memory allocation (for example, a dynamically growing array or linked list) instead of a fixed-size array. The trade-offs include added complexity in managing memory (handling reallocations, ensuring proper deallocation) and potential performance overhead, balanced against the increased flexibility to handle any number of commands
