1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  fork let's us execute the command in a seperate child process, as not to disturb our shell. We dont want to have to leave our shell.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  the pid will be < 0 so we exit()

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**: execvp finds the command in the system env PATH variable  

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  the parent process would move on without waiting for the child to finish, so we would potentially skip past the output of the command. It would cause alot of issues too

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  this gives us the exit status of the child process letting us know if our command executed correctly

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**: when I get to a quote I simply moved the pointer until I hit another quote, as not to disturb the input provided. I don't want to change the string the user gave

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  I completely rewrote my code and it was 10x simpler. No challenges, just surprisingly simple

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals in a Linux system are used for asynchronous interprocess communication, allowing processes to handle events like termination or interruptions. Unlike other IPC methods (e.g., pipes, shared memory, message queues), signals are lightweight and do not require explicit data exchange between processes.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL (9) – Immediately terminates a process and cannot be caught or ignored. Used to forcefully stop unresponsive processes.
                   SIGTERM (15) – Politely asks a process to terminate, allowing it to clean up before exiting. Default signal for kill.
                   SIGINT (2) – Sent when a user presses Ctrl+C in the terminal to interrupt a process.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP (19), it is suspended (paused) until it receives SIGCONT to resume execution. Unlike SIGINT, it cannot be caught, ignored, or blocked because it is designed to be an unpreventable control mechanism for stopping processes.
