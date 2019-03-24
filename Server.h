//
// Created by omar_swidan on 24/03/19.
//

#ifndef SOCKET_PROGRAMMING_SERVER_H
#define SOCKET_PROGRAMMING_SERVER_H


#include <sys/socket.h>
#include <iostream>
#include <error.h>
#include <netinet/in.h>
#include <cstring>
class Server {

     struct addrinfo hints;

public:
    void start();
};


#endif //SOCKET_PROGRAMMING_SERVER_H
