*This project has been created as part of the 42 curriculum by tcarlier, igilbert.*

# ft_irc

## Description

This project is a custom Internet Relay Chat (IRC) server entirely developed in C++98. The primary goal of this project is to understand and implement a robust, multiplexed server capable of handling multiple concurrent client connections using non-blocking I/O operations and the `poll()` system call.

By conforming to the standard IRC protocol, this server allows real users to connect using official IRC clients (such as irssi, WeeChat, or nc), join channels, exchange messages, and manage channel administration through various operator modes.

**Key Features:**

* Fully non-blocking server architecture using `poll()`.


* Authentication mechanism via a server password.


* Private messaging (`PRIVMSG`) and silent notifications (`NOTICE`).


* Full channel management (`JOIN`, `PART`, `KICK`, `TOPIC`, `INVITE`).


* Support for channel operator modes:
* `+i`: Invite-only channel.


* `+t`: Topic modification restricted to channel operators.


* `+k`: Password-protected channel.


* `+o`: Grant/revoke channel operator privileges.


* `+l`: Set a limit on the maximum number of users in a channel.





## Instructions

### Compilation

The project includes a Makefile to compile the server. The code strictly adheres to the C++98 standard and is compiled using `c++` with the `-Wall -Wextra -Werror` flags.

To compile the project, run the following command at the root of the repository:

```bash
make

```

Other available rules:

* `make clean`: Removes object files.
* `make fclean`: Removes object files and the executable.
* `make re`: Recompiles the entire project from scratch.

### Execution

Once compiled, the server executable is named `ircserv`. It requires two arguments to run: the listening port and the connection password.

```bash
./ircserv <port> <password>

```

* **`<port>`**: The network port on which the server will listen for incoming connections (must be between 1024 and 49151).


* **`<password>`**: The password required for clients to authenticate and connect to the server.



### Connecting a Client

You can connect to the server using a standard IRC client like `HexChat`:

Or simply using `nc` (netcat) for raw protocol testing:

```bash
nc 127.0.0.1 <port>
PASS <password>
NICK <your_nickname>
USER <username> 0 * :<Real Name>

```

## Resources

The development of this project relied on standard networking documentation and official protocol specifications:

* **[RFC 1459: Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)**: The foundational document for understanding the architecture and commands of IRC.
* **[RFC 2812: Internet Relay Chat: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)**: The updated specification for client-server communication, numerical replies, and modern IRC features.
* **Linux man pages**: Extensive use of `man 2 poll`, `man 2 socket`, `man 2 bind`, `man 2 accept`, and `man 2 fcntl` for the underlying network architecture.

**Use of Artificial Intelligence:**
An AI assistant (Gemini) was used during the development of this project in a mentoring and debugging capacity.