/*******************************************************************************
*      Filename: chatclient.c
*        Author: Maxwell Goldberg
* Last Modified: 02.12.17
*   Description: The main chatclient method file.
*******************************************************************************/

#include "validate.h"
#include "network.h"

/*******************************************************************************
* Function: main()
* Description: Establishes a connection with the server socket specified on the
*              command line. Exchanges messages in a loop with the server until
*              the connection is closed or the user enters '\quit'.
* Parameters: int argc - The command line argument count.
*             char *argv[] - The string array of command line arguments.
* Preconditions: None.
* Returns: 0 on success. Positive integers on failure.
*******************************************************************************/

int main(int argc, char *argv[]) {
    int  status, sockfd;
    char handle[MAX_BYTES];
    char buffer[MAX_BYTES * 2];

    /* Validate the command line arguments. */
    validateArgs(argv[1], argv[2], argc);
    /* Get the user handle and validate it. */
    createValidatedHandle(handle);
    /* Form the socket and connect it to the server. */
    sockfd = formConnection(argv[1], argv[2]);
   
    /* Loop until the user inputs '\quit'. */
    while(1) { 
        /* Get a validated user message. If the user input is '\quit', the
         * return value is 0, so we break out of the loop. */
        if (createValidatedMsg(handle, buffer, sizeof buffer) == 0) {
            break;
        }
        /* Send the message. */
        chatSend(sockfd, buffer, strlen(buffer)+1);
        /* Receive a message from the user. If the return value is 0,
           the connection has been broken. */
        if (chatReceive(sockfd, buffer) == 0) {
            break;
        }
        /* Print the received message. */
        printf("%s\n", buffer);
    }
    /* Close the socket. */
    close(sockfd); 
    printf("Socket closed. Exiting chatclient.\n");

    return 0;
}


