1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() is a good choice because it allows you to read in from a specified file stream (in our case stdin) until it hits EOF or is terminated early. This allows us to have it constantly read until we are ready to submit input.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**: We used malloc because user input varies in length, and just because it can be <= max_length doesnt mean it will be. We could have filled the array of max_length symbols like "." and then memcopied it later, but that would be extra work. malloc let's us vary the length


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  varied length of commands based on unpredictable white space length.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  we should implement file output redirection, process return data, and process errors. we can redirect file output with fgets, but to redirect a process's return value will be difficult because our program needs to be the one to call it (I think). We can't cheat and run it through bash. I think we will use execv sys call for this but idk.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  pipes are used to redirect output to a process or utility, redirection (> or >>) is used to send output to a file or stream

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  _start here_

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  _start here_
