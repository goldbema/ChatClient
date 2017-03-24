# Chat Client and Server

This is a simple client-server architecture that uses TCP sockets to perform ASCII-text message transfer on UNIX-based systems.

## Compilation

In the working directory containing the program files, type ``make``. This will run a simple ``makefile`` that compiles an executable labeled ``chatclient``.

In a separate terminal, ensure that ``chatserve`` has executable permissions by typing ``chmod +x chatserve``.

## Execution

### Initial Steps

1. In the server terminal, execute ``chatserve`` by entering ``chatserve port`` where ``port`` is an integer from 1 to 65535 inclusive. ``chatserve`` will exit with an error message if an invalid port number is specified.
2. On successful execution, ``chatserve`` will print ``Server listening on port <PORT>...``.
3. To execute ``chatclient``, enter ``chatclient server_hostname port``, where ``server_hostname`` is the server hostname and ``port`` is the port specified to ``chatserve`` in step 1.
4. ``chatclient`` will prompt the user to enter the client handle. This can be any sequence of alphanumeric characters and underscores up to 10 characters in length. The handle cannot be empty. ``chatclient`` will repeatedly ask for the handle until it is given a valid handle.

### Sending chat messages

5. On successful handle entry, ``chatclient`` will display the handle as a prompt. The user may now enter a chat message of 0-500 characters inclusive. Messages longer than 500 characters will cause ``chatclient`` to display `chatclient: Invalid message length` and to prompt the user for a new message. Valid messages will be output on the ``chatserve`` window with the client handle + `> ` prepended to them.
	* Note: If the user enters `\quit`, ``chatclient`` will display `Socket closed. Exiting chatclient` and exit the ``chatclient`` program.
6. On the server window, the user may now enter a chat message of 0-500 characters inclusive. Messages longer than 500 characters are treated similarly to those in step 5. If the user enters `\quit`, then ``chatserve`` will return to listening for new client connections. If a valid message other than `\quit` is entered, the message will be output on ``chatclient`` with `chatserve> ` prepended to it.

### Exiting ``chatclient`` and ``chatserve``

7. Repeat from step 5 if neither user has entered ``\quit`` and the connection has not closed unexpectedly. The ``chatclient`` will handle ``chatserver`` ``\quit`` and unexpected connection closures by ending execution after closing its own socket. ``chatserve`` will handle ``chatclient`` ``\quit`` by closing its chat socket and returning to a listening state.
8. After the ``chatclient`` has exited, ``chatserve`` can be exited by typing ``Ctrl-C``.

## Cleaning up

9. Once both ``chatserve`` and ``chatclient`` have finished executing, the executable ``chatclient`` can be removed by entering ``make clean`` into the ``chatclient`` terminal.

© Maxwell Goldberg 2017
