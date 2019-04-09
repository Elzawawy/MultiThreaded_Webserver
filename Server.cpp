//
// Created by omar_swidan on 24/03/19.
//


#define __USE_XOPEN2K
#define _POSIX_C_SOURCE 200112L
#include <strings.h>
#include <sys/types.h>
#include <netdb.h>

#include "Server.h"
using  std::cout;

void Server::start() {

//preparing to launch
    addrinfo hints,*results;
    int status;
    //make sure struct hints is empty
    memset(&hints,0,sizeof hints);

    hints.ai_family=AF_INET;            //IPv4
    hints.ai_socktype=SOCK_STREAM;      //TCP
    hints.ai_flags=AI_PASSIVE;          //fill in my ip for me


    status = getaddrinfo(nullptr,"80",&hints,&results);
    if(status != 0 )            //status is nonzero in case of error.
        cout<<"error";
    cout<<results;

    //creating the socket
    int socketRes=socket(AF_INET,SOCK_STREAM,0);
    if(socketRes<0)
        cout<<"error";



    //bind(socketRes,INADDR_ANY,)//INADDR_ANY is the current host ip address

}
