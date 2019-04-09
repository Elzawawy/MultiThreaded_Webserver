//
// Created by omar_swidan on 24/03/19.
//


#include <strings.h>
#include <sys/types.h>
#include <netdb.h>
#include "Server.h"
using  std::cout;

void Server::start() {

    //Step1: Preparing to Launch using getaddrinfo().
    addrinfo hints,*results;
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
    cout<<results;
    //Reminder to use freeaddrinfo(results); in the end of the work.

    //creating the socket
    int socketRes=socket(AF_INET,SOCK_STREAM,0);
    if(socketRes<0)
        cout<<"error";

    //bind(socketRes,INADDR_ANY,)//INADDR_ANY is the current host ip address

}
