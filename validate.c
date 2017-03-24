/*******************************************************************************
*      Filename: validate.c
*        Author: Maxwell Goldberg
* Last Modified: 02.12.17
*   Description: A file containing a variety of validation functions, including
*                command line argument validate, hostname and port validation, 
*                client handle validation, and message validation.
*******************************************************************************/

#include "validate.h"

/*******************************************************************************
* Function: validateArgs()
* Description: Validates the command line arguments passed to chatclient.
* Parameters: char *hostname - The hostname to be validated.
*             char *port - The port string to be validated.
*             int numArgs - The number of command line arguments passed.
* Preconditions: None.
* Returns: 1 if both arguments are valid, 0 otherwise. 
*******************************************************************************/

int validateArgs(char *hostname, char *port, int numArgs) {
    /* If more than two arguments in addition to 'chatclient' are passed,
     * return with an error.
     */
    if (numArgs != 3) {
        fprintf(stderr, "usage: chatclient hostname port\n");
        exit(1);
    }
    return (validateHostname(hostname) && validatePort(port));
}

/*******************************************************************************
* Function: validateHostname()
* Description: Validates the hostname string passed on the command line
*              according to guidelines set in RFC 1123.
* Parameters: char *hostname - The hostname string to be evaluated.
* Preconditions: None.
* Returns: 1 if the string is valid, exits on failure.
*******************************************************************************/

int validateHostname(char *hostname) {
    char *str = hostname;
    int labelLen = 0;
    int strLen = 0;

    while (*str != '\0') {
        /* The hostname may contain only alphanumerics, periods, and dashes. */
        if (!isdigit(*str) && !isalpha(*str) && (*str != '-') &&
            (*str != '.')) {
            fprintf(stderr, "chatclient: invalid hostname\n");
            exit(1);
        }
        /* If we hit a period, the label has ended, so reset the label length.*/
        if (*str == '.') {
            labelLen = 0;
        /* Otherwise, increment it. */
        } else {
            labelLen++;
        }
        strLen++;
        /* Validate label length. */
        if (labelLen > MAX_HOST_LABEL) {
            fprintf(stderr, "chatclient: hostname label len is at most 63\n");
            exit(1);
        }
        /* Validate hostname length. */
        if (strLen > HOST_NAME_MAX) {
            fprintf(stderr, "chatclient: hostname length must be less than \
                             or equal to HOST_NAME_MAX\n");
            exit(1);
        }

        str++;
    }
    /* Ensure that the first and last characters are alphanumeric. */
    if ((hostname[0] == '.') || (hostname[strLen-1] == '.')) {
        fprintf(stderr, "chatclient: hostname cannot begin or end with '.'\n");
        exit(1);
    }
    if ((hostname[0] == '-') || (hostname[strLen-1] == '-')) {
        fprintf(stderr, "chatclient: hostname cannot begin or end with '-'\n");
        exit(1);
    }

    return 1;
}

/*******************************************************************************
* Function: validatePort()
* Description: Validates the command line port number argument.
* Parameters: char *port - The port string to be evaluated.
* Preconditions: None.
* Returns: 1 on success, exits on failure.
*******************************************************************************/

int validatePort(char *port) {
    char *str;
    /* Uset strtol to conver the port number to a long integer. */
    long portVal = strtol(port, &str, 10);
    /* If the new string pointer location does not point to the null terminator,
     * then we know additional characters exist in the string or that the string
     * didn't lead with an integer.
     */
    if(*str != '\0') {
        fprintf(stderr, "chatclient: port must contain only digits\n");
        exit(-1);
    }
    /* Validate the integer. */
    if ((portVal < MIN_PORT) || (portVal > MAX_PORT)) {
        fprintf(stderr, "chatclient: port must be between %d and %d\n",
                MIN_PORT, MAX_PORT);
        exit(-1);
    }
    /* Print the port number to the port string (now used as a buffer. */
    snprintf(port, MAX_PORT_CHARS+1, "%ld", portVal);
 
    return 1;
}

/*******************************************************************************
* Function: validateHandle()
* Description: Validates a user-specified handle.
* Parameters: char *handle - The handle string to be evaluated.
* Preconditions: The handle has been taken in through fgets() and the string
*                buffer is large enough to hold the largest handle possible. 
* Returns: 1 if the handle is valid, 0 otherwise.
*******************************************************************************/

int validateHandle(char *handle) {
    char *str = handle;
    int strLen = 0;
    while (*str != '\0' && *str != '\n') {
        /* The handle can only containe alphanumerics and underscores. */
        if (!isalnum(*str) && (*str != '_')) {
            fprintf(stderr, 
               "chatclient: handle must contain only alphanumerics or '_'\n");
            return 0;
        }
        /* Validate the handle length. */
        strLen++;
        if (strLen > MAX_HANDLE_LEN) {
            fprintf(stderr, 
               "chatclient: handle must contain fewer than 10 chars\n");
            return 0;
        }
        str++;
    }
    /* Ensure the handle isn't empty. */
    if (strLen == 0) {
        fprintf(stderr, "chatclient: handle cannot be empty\n");
        return 0;
    }
    /* Append the null terminator in place of the user-entered '\n' */
    handle[strLen] = '\0';
 
    return 1;

}

/*******************************************************************************
* Function: createValidatedHandle()
* Description: Takes in user input on the handle while the handle is invalid.
* Parameters: char *handle - The handle character string buffer.
* Preconditions: The handle buffer is large enough to accept MAX_MSG characters.
* Returns: None.
*******************************************************************************/

void createValidatedHandle(char *handle) {
    do {
        /* Get user input. */
        printf("Please enter the client handle: ");
        fgets(handle, MAX_MSG, stdin);
        /* If the newline char can't be found in the input, there is still
         * input in stdin, so consume it. */
        if (!strchr(handle, '\n')) {
            while (fgetc(stdin) != '\n') { }
        }
        /* Validate user input. */
    } while (!validateHandle(handle));
}

/*******************************************************************************
* Function: _validateMsg()
* Description: A helper function for createValidatedMsg(). Validates the message
*              length.
* Parameters: char *msg - The message string.
* Preconditions: None.
* Returns: 1 on successful validation, 0 otherwise.
*******************************************************************************/

int _validateMsg(char *msg) {
    /* If the message has the correct length, return 1. */
    if (msg && (strlen(msg) <= MAX_MSG)) {
        return 1;
    }
    /* Otherwise, print an error and return 0. */
    fprintf(stderr, "chatclient: Invalid message length\n");
    return 0;
}

/*******************************************************************************
* Function: _prependByteCountMsg()
* Description: Prepends the message body's byte count to the message body.
* Parameters: char *msg - The message buffer.
*             int msgLen - The length of the message body. 
* Preconditions: The message buffer has been correctly initialized, and its size
*                is correctly reflected by msgLen.
* Returns: None.
*******************************************************************************/

void _prependByteCountMsg(char *msg, int msgLen) {
    /* Place the length of the message body into an empty buffer. */
    char buffer[MAX_BYTES];
    sprintf(buffer, "%03d", strlen(msg)+1);
    /* Concatenate the msg with the buffer. */
    strcat(buffer, msg);
    /* Copy back the buffer to the message. */
    memset(msg, 0, msgLen);
    strcpy(msg, buffer);
}

/*******************************************************************************
* Function: createValidatedMsg()
* Description: Displays the message prompt, takes in input, validates it, and
*              creates the message header. Loops on input validation failure.
* Parameters: char *handle - The handle string.
*             char *msg - The message string buffer.
*             int msgBufferLen - The length of the message buffer.
* Preconditions: The handle has been validated, the message buffer has been
*                initialized, and the length accurately reflects the message
*                buffer length.
* Returns: 0 if the user opts to \quit, 1 otherwise.
*******************************************************************************/

int createValidatedMsg(char *handle, char *msg, int msgBufferLen) {
    char buffer[MAX_MSG*2];
    int bufferLen, handleLen;
    do { 
        /* Reset the buffers. */
        memset(buffer, 0, sizeof buffer);
        memset(msg, 0, msgBufferLen);
        /* Place the handle in the message buffer. */
        sprintf(msg, "%s> ", handle);
        printf("%s", msg);
        /* Take user input into the local buffer. */
        fgets(buffer, MAX_MSG*2-strlen(msg), stdin);
        /* Concatenate the buffer into msg. */
        strcat(msg, buffer);
        /* Consume charaters from stdin if necessary. */
        if (!strchr(msg, '\n')) {
            while (fgetc(stdin) != '\n') { }
        /* Otherwise, replace the input '\n' with a null terminator. */
        } else { 
            msg[strlen(msg)-1] = 0;
        }
        /* Remove newline char from the buffer and see if the formatted
         * input is '\quit'. If so, return 0. 
         */ 
        bufferLen = strlen(buffer);
        if (bufferLen > 0 && buffer[bufferLen-1] == '\n') {
            buffer[bufferLen-1] = '\0';
        }
        if (strcmp(buffer, "\\quit") == 0) {
            return 0;
        }
        /* Repeat the loop if the message body is invalid. */
    } while (!_validateMsg(buffer));
    /* Prepend the byte count header to the message. */
    _prependByteCountMsg(msg, strlen(buffer));
    return 1;
}
