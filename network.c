/*******************************************************************************
*      Filename: network.c
*        Author: Maxwell Goldberg
* Last Modified: 02.12.17
*   Description: Provides functions that allow the chatclient to form a TCP
*                connection with the chatserver, send a chat message, and 
*                receive a chat message.
*   Attribution: The functions in this file (especially formConnection) are 
*                based on socket code available in Beej's Guide
*                to Network Programming by Brian Hall (beej.us/guide/bgnet/output/
*                html/multipage/index.html).
*******************************************************************************/

#include "network.h"
#include "validate.h"

/*******************************************************************************
* Function: formConnection()
* Description: Creates a TCP socket at a system-selected port, attempts to
*              connect to the hostname and port passed as parameters, and
*              returns the socket file descriptor on success and exits with an
*              error on failure. Please note that this code is based on code
*              from Beej's Guide available under the header "A Simple Stream
*              Client."
* Parameters: char *host - A string containing the hostname.
*             char *port - A string containing the port number.
* Preconditions: The hostname and port have been properly validated.
* Returns: The socket file descriptor of the connected socket on success.
*******************************************************************************/

int formConnection(char *host, char *port) {
    struct addrinfo hints, *res, *p;
    int status, sockfd;

    /* Prefill the hints addrinfo struct with the SOCK_STREAM socket type and
     * don't specify whether the address is IPv4 or IPv6.
     */

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    /* Attempt to us getaddrinfo() to fill in the res addrinfo struct. If there
     * is an error, print it to stderr and exit with an error.
     */

    if ((status = getaddrinfo(host, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(2);
    }

    /* getaddrinfo() may return more than one potential address match, so we
     * need to iterate through the res linked list in order to find the first
     * one that allows us to connect to it.
     */

    for (p = res; p != NULL; p = p->ai_next) {
        /* Attempt to create a socket with the correct address family, socket
         * type (SOCK_STREAM), and protocol (TCP). If this operation fails,
         * print an error and go to the next address candidate.
         */
        if ((sockfd = socket(p->ai_family, p->ai_socktype, 
                             p->ai_protocol)) == -1) {
            perror("chatclient: socket");
            continue;
        }
        /* Attempt to connect the created socket to the current address
         * candidate. If an error occurs, print it and exit. Otherwise,
         * we can break out of the loop.
         */
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("chatclient: connect");
            continue;
        }
        break;
    }

    /* Free the memory allocated for the res addrinfo struct. */
    freeaddrinfo(res);

    /* If we've reached the end of the linked list without finding a match,
     * then print an error and exit. 
     */
    if (p == NULL) {
        fprintf(stderr, "chatclient: failed to connect\n");
        exit(2);
    }

    return sockfd;
}

/*******************************************************************************
* Function: chatSend()
* Description: Attempts to send the entirety of a chat message to the server.
* Parameters: int sockfd - The socket file descriptor.
*             char *message - A character array containing the message.
*             int msgLen - The length of the message in bytes.
* Preconditions: The socket has been correctly formed. The message has been
*                properly validated, and its length is accurate.
* Returns: None.
*******************************************************************************/

void chatSend(int sockfd, char *message, int msgLen) {
    /* We will keep track of the total number of bytes send as well as the 
     * bytes sent by a current instance of send().
     */
    int totalSent = 0;
    int currSent;
    /* While there are still bytes to be sent...*/
    while (totalSent < msgLen) {
        /* Send the remainder of the message to the server. If an error 
         * occurs, exit with an error message.
         */
        if ((currSent = send(sockfd, &message[totalSent], msgLen-totalSent,
                             0)) == -1){
            perror("send");
            exit(-1);
        }; 
        /* Increment the total amount of bytes sent. */
        totalSent += currSent;
    }
}

/*******************************************************************************
* Function: _chatReceiveHelper()
* Description: An ancillary function for chatReceive() that is used to first 
*              obtain the number of bytes int the body of the message
*              and then in a separate instance used to obtain the message body.
* Parameters: int sockfd - The socket file descriptor.
*             char *message - The message string buffer.
*             int msgLen - The length of the message to be received.
* Preconditions: The socket has been initialized. The message buffer is msgLen
*                in size, and msgLen is accurate.
* Returns: An error status on failure of recv(). Otherwise, the final error 
*          status of the successful recv() is returned. The result is stored in
*          the message buffer.
*******************************************************************************/

int _chatReceiveHelper(int sockfd, char *message, int msgLen) {
    char buffer[MAX_BYTES];
    int bytesReceived = 0;
    int status;

    /* While not all bytes in the current message section have been received,
     * reset the local string buffer, call recv() to store new characters in
     * the buffer, and concatenate the buffer to the message.
     */
    while (bytesReceived < msgLen) {
        memset(buffer, 0, sizeof buffer);

        status = recv(sockfd, buffer, msgLen - bytesReceived, 0);
        /* If no bytes are received print an error and return the error
         * status.
         */
        if (status == 0) {
            printf("Server ended connection.\n");
            return status;
        }
        strcat(message, buffer);
        bytesReceived += status;
    }

    return status;
}

/*******************************************************************************
* Function: chatReceive()
* Description: Receives the chat header of three characters indicating the 
*              length of the message body to follow, then receives the chat 
*              message body.
* Parameters: int sockfd - The socket file descriptor.
*             char *message - The message buffer.
* Preconditions: The socket has been correctly initialized. The message buffer
*                is large enough to hold the maximal message size.
* Returns: A status indicating failure of the recv() function. 0 indicates 
*          failure, and any other value represents success.
*******************************************************************************/

int chatReceive(int sockfd, char *message) {
    char header[PREFIX_OFFSET+1];
    char body[MAX_BYTES];
    long msgLen; 
    int status;
    /* Initialize the local string buffers. */
    memset(header, 0, sizeof header);
    memset(body, 0, sizeof body);
    /* Get the numeric message prefix. */ 
    status = _chatReceiveHelper(sockfd, header, PREFIX_OFFSET);
    /* If failure occurs, return immediately. */
    if (status == 0) {
        return status;
    }
    /* Convert the message length to a number. */
    msgLen = strtol(header, NULL, 10);
    /* Use this number to receive the message body. */
    status = _chatReceiveHelper(sockfd, body, msgLen);
    /* Copy the local buffer to the message buffer parameter. */
    strcpy(message, body);
    /* Return the status returned by the helper function. */
    return status;
}
