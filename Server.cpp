//
// Created by omar_swidan on 24/03/19.
//


#include <strings.h>
#include <sys/types.h>
#include <netdb.h>
#include "Server.h"
#include <arpa/inet.h>
#include <netinet/in.h>
using  std::cout;

void Server::start() {

    //Step1: Preparing to Launch using getaddrinfo().
    struct addrinfo hints,*results;
    struct sockaddr_storage their_addr;
    int status;
    //make sure struct hints is empty
    memset(&hints,0,sizeof hints);
    //set your needed parameters of Server.
    hints.ai_family=AF_INET;            //IPv4
    hints.ai_socktype=SOCK_STREAM;      //TCP
    hints.ai_flags=AI_PASSIVE;          //fill in my ip for me
    //Does all kinds of good stuff for us, including DNS and service name lookups, and fills out the structs we need.
    //Params: nullptr is for IP address for local host, 80 is port number ,hints is our required params.
    //results points to a linked list of struct addrinfo.
    status = getaddrinfo(nullptr,"80",&hints,&results);
    if(status != 0 )            //status is nonzero in case of error.
        cout<<gai_strerror(status);
    struct addrinfo *p;
    char ipstr[INET6_ADDRSTRLEN];
    for(p = results;p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;
        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }
        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf(" %s: %s\n", ipver, ipstr);
    }
    //Reminder to use freeaddrinfo(results); in the end of the work.



    //creating the socket
    // simply returns to you a socket descriptor that you can use in later system calls, or -1 on error.
    int sockRes=socket(results->ai_family,results->ai_socktype,results->ai_protocol);
    if(sockRes<0)
        cout<<"error";
    // bind it to the port we passed in to getaddrinfo():
    int bindRes = bind(sockRes, results->ai_addr, results->ai_addrlen);
    if(bindRes < 0)
        cout<<"error";
    //cout<<bindRes;
    int lisRes = listen(sockRes,5);
    if(lisRes < 0)
        cout<<"error";
    // now accept an incoming connection:

    socklen_t addr_size = sizeof their_addr;
    int new_fd = accept(sockRes, (struct sockaddr *)&their_addr, &(addr_size));

}
