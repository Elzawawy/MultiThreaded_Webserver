//
// Created by omar_swidan on 24/03/19.
//


#include <strings.h>
#include <sys/types.h>
#include <netdb.h>
#include "Server.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <sstream>

#define MAXDATASIZE 1000 // max number of bytes we can get at once
#define MY_PORT "80"
#define BACK_LOG 10
#pragma clang diagnostic push
#pragma clang diagnostic pop
#pragma clang diagnostic ignored "-Wmissing-noreturn"
using namespace std;
string getResponse(char* request);

void clientHandler(struct sockaddr_storage* their_addr,int client_socket)
{
    pthread_detach(pthread_self()); /* terminates on return */
//    char string[INET_ADDRSTRLEN];
//    socklen_t size = sizeof string;
//    inet_ntop
//    inet_ntop(their_addr->ss_family, &(((struct sockaddr_in*)((struct sockaddr *)&their_addr)))->sin_addr),string,size);
//    printf("server: got connection from %s\n", string);
    cout<<"Connected to a client !"<<endl;
    /* request */
    char buffer[MAXDATASIZE + 1];
    bzero(buffer, sizeof(buffer));
    int bytes_read = static_cast<int>(recv(client_socket, buffer, sizeof(buffer), 0));
    if(bytes_read < 0)
    {
        perror("recv");
        exit(1);
    }

    string response = getResponse(buffer);

}

void Server::start() {

    //Step1: Preparing to Launch using getaddrinfo().
    struct addrinfo hints,*results, *temp;
    struct sockaddr_storage their_addr;
    int status;
    int sock_fd, new_sockfd;
    int numBytes;
    char buf[MAXDATASIZE];
    //make sure struct hints is empty
    memset(&hints,0,sizeof hints);
    //set your needed parameters of Server.
    hints.ai_family=AF_INET;            //IPv4
    hints.ai_socktype=SOCK_STREAM;      //TCP
    hints.ai_flags=AI_PASSIVE;          //fill in my ip for me
    //Does all kinds of good stuff for us, including DNS and service name lookups, and fills out the structs we need.
    //Params: nullptr is for IP address for local host, 80 is port number ,hints is our required params.
    //results points to a linked list of struct addrinfo.
    if ((status = getaddrinfo(nullptr, MY_PORT, &hints, &results)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }
    // loop through all the results and bind to the first we can
    for(temp = results; temp != nullptr; temp = temp->ai_next) {
        if ((sock_fd = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }
        if (bind(sock_fd, temp->ai_addr, temp->ai_addrlen) == -1) {
            close(sock_fd);
            perror("bind");
            continue;
        }
        break; // if we get here, we must have connected successfully
    }
    // looped off the end of the list with no successful bind
    if (temp == nullptr) {
        fprintf(stderr, "failed to bind socket\n");
        exit(2);
    }
    freeaddrinfo(results); // all done with this structure

    //This is what differentiates the servers from the clients
    if (listen(sock_fd, BACK_LOG) == -1) {
        perror("listen");
        exit(1);
    }
    cout<<"Hi there ! I am Listening !"<<endl;
    while(true) { //Main loop
        socklen_t sin_size = sizeof their_addr;
        new_sockfd = accept(sock_fd, (struct sockaddr *)&their_addr, &sin_size);
        if(new_sockfd == -1)
        {
            perror("accept");
            continue;
        }
        thread th(clientHandler,&their_addr,new_sockfd);
    }

}

string getResponse(char* request)
{


}



