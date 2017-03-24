/*******************************************************************************
*      Filename: validate.h
*        Author: Maxwell Goldberg
* Last Modified: 02.12.17
*   Description: The header file for validate.c. Please see validate.c for more
*                details on each function.
*******************************************************************************/

#ifndef VALIDATE_H
#define VALIDATE_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MIN_PORT       1
#define MAX_PORT       65535
#define MAX_PORT_CHARS 5
#define MAX_HOST_LABEL 63
#define MAX_HANDLE_LEN 10
#define MAX_BYTES      516
#define MAX_MSG        500
#define PREFIX_OFFSET  3

int validateArgs(char *, char *, int);
int validateHandle(char *);
int validateHostname(char *);
int validatePort(char *);
void createValidatedHandle(char *);
int createValidatedMessage(char *, char *, int);

#endif
