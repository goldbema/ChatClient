/*******************************************************************************
*      Filename: network.h
*        Author: Maxwell Goldberg
* Last Modified: 02.12.17
*   Description: The header file for network.c. Please see network.c for more 
*                details.
*******************************************************************************/

#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int formConnection(char *, char *);
void chatSend(int, char *msg, int);
int chatReceive(int, char *);

#endif
