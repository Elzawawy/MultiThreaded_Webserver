//
// Created by omar_swidan on 24/03/19.
//
#include<iterator>
#include <netdb.h>
#include <cstring>
#include <cstdio>
#include <string>

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <sstream>


#include <fstream>
#include "Client.h"
#define MAXDATASIZE 1000

int Client::start(string input){

    vector<string>* tokens=parse_string(input);

    string *message=make_message(tokens->at(0),tokens->at(1),tokens->at(2));

    char buff[MAXDATASIZE];

    int status;
//    char *host =new char[tokens->at(2).length()+1];
//    strcpy(host,tokens->at(2));
//
    char *host= (char *) "www.google.com";
    //char* host= (char *) "www.google.com";

    struct addrinfo hints,*p,*res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;//ipv4
    hints.ai_socktype = SOCK_STREAM;//TCP

    //status = getaddrinfo(host, "3490", &hints, &servinfo);//get ready to connect



    if ((status = getaddrinfo(host, "80", &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    char ipstr[INET6_ADDRSTRLEN];
   //Printing the contents of the linked list
    printf("IP addresses for %s:\n\n", host);


    for(p = res;p != NULL; p = p->ai_next) {

        void *addr = nullptr;
        char *ipver;
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = (char *) "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = (char *) "IPv6";
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf(" %s: %s\n", ipver, ipstr);
    }

    //getting socket descriptor for upcoming system calls
    int socket_descriptor = socket(res->ai_family, res->ai_socktype, res->ai_protocol);


    if (connect(socket_descriptor, res->ai_addr, res->ai_addrlen) == -1) {
        close(socket_descriptor);
        perror("client: connect");
    }


    //send request message
    int len, bytes_sent;

    len = (int) message->length();
    bytes_sent = (int) send(socket_descriptor, message, (size_t) len, 0);

    // receive message

    int numbytes;
    if ((numbytes = (int) recv(socket_descriptor, buff, MAXDATASIZE - 1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buff[numbytes] = '\0';

    string s{buff};
    cout<<buff;

    close(socket_descriptor);
    freeaddrinfo(res); // free the linked list

    return 0;

}


string *Client::make_message(string request_type,string filename,string hostname) {

    string *message=new string();
    message->append(request_type+" "+filename+" "+"HTTP/1.0"+"\r\n");

    message->append("\r\n");
    if(request_type=="POST"){

        streampos size;
        char * memblock;

        ifstream post_file(filename,ios::binary|ios::in|ios::ate);
        if (post_file.is_open())
        {

            size = post_file.tellg();
            memblock = new char [size];

            post_file.seekg (0, ios::beg);
            post_file.read (memblock, size);
            post_file.close();

            message->append(memblock);

            cout<<sizeof(memblock)<<" "<<size<<endl;
            delete[] memblock;


        }
    }
    return message;
}

vector<string>* Client:: parse_string(string input)
{


    istringstream iss(input);
    vector<string> *results = new vector<string>;

    copy(istream_iterator<string>(iss),
    istream_iterator<string>(),
    back_inserter(*results));

    return results;

}

void Client::get_data(string message) {
    for (auto i=message.begin(); i !=message.end() ; ++i) {

    }

}




