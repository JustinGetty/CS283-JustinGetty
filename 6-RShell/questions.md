1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The client keeps reading data until it sees a special marker (the EOF character, 0x04) at the end. This marker tells it that the full message has been sent. Techniques include using such delimiters or sending a length prefix so that the client knows exactly how many bytes to expect.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

Because TCP only provides a continuous stream of bytes, the protocol must add its own boundaries—typically by appending a specific delimiter (like a null byte or EOF marker) at the end of each command. If not handled correctly, the receiver might read partial messages or combine separate messages, causing misinterpretation or errors.

3. Describe the general differences between stateful and stateless protocols.

In a stateful protocol, the server keeps information (state) about previous interactions, so each new request can depend on past requests. In a stateless protocol, every request is independent, and the server does not remember any previous interactions.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDP is faster and has lower overhead than TCP, making it ideal for applications where speed is more important than perfect reliability, such as streaming video, online gaming, or voice over IP. It also supports multicast and broadcast, which TCP does not.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The operating system provides the socket API, which lets applications create sockets and use functions like socket(), bind(), listen(), accept(), connect(), send(), and recv() to communicate over the network.
