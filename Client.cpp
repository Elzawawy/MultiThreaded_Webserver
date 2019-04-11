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


int Client::start(string input){

    vector<string>* tokens=parse_string(input);
    string *message=make_message(tokens->at(0),tokens->at(1),tokens->at(2));

    int status;

    char* host= (char *) "www.google.com";
    struct addrinfo hints,*p,*res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;//ipv4
    hints.ai_socktype = SOCK_STREAM;//TCP



    //status = getaddrinfo(host, "3490", &hints, &servinfo);//get ready to connect
    char ipstr[INET6_ADDRSTRLEN];


    if ((status = getaddrinfo(host, "80", &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }



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

    //Demo send code
    char *msg = (char *) "Beej was here!";
    int len, bytes_sent;

    len = (int) strlen(msg);
    bytes_sent = (int) send(socket_descriptor, msg, (size_t) len, 0);

    cout<<len<<" "<<bytes_sent;

    close(socket_descriptor);

    freeaddrinfo(res); // free the linked list

    return 0;

}


string *Client::make_message(string request_type,string filename,string hostname) {

    string *message=new string();
    message->append(request_type+" "+filename+" "+"HTTP/1.0"+"\n");
    message->append("Host:"+hostname+"\n");
    message->append("\n");
    if(request_type=="POST"){
        streampos size;
        char * memblock;

        ifstream post_file(filename,ios::binary);
        if (post_file.is_open())
        {
            size = post_file.tellg();
            memblock = new char [size];
            post_file.seekg (0, ios::beg);
            post_file.read (memblock, size);
            post_file.close();

            cout << "the entire file content is in memory";

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
    cout << results->at(1);
    return results;

}




