//
// Created by omar_swidan on 24/03/19.
//
#include<iterator>
#include <netdb.h>
#include <cstring>

#include <arpa/inet.h>

#include <iostream>
#include <unistd.h>
#include <sstream>


#include <fstream>
#include "Client.h"
#define MAXDATASIZE 1000

int Client::start(string input){

    vector<string>* tokens=parse_string(input);

    string *message=make_message(tokens->at(0),tokens->at(1),tokens->at(2));
    string message_type=(tokens->at(0));
    char buff[MAXDATASIZE];

    int status;
//    const char *host =(tokens->at(2)).c_str();


    char *host= (char *) "www.google.com";


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
    string *message_body = nullptr;

   if(get_status_code(string{buff})==200) {
       if (message_type == "GET")
           message_body = (get_data(string{buff}));
       else
           *message_body = "message posted";
       write_to_file(*(message_body),"get_file");
   } else cout<<"request error"<<endl;


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

string* Client::get_data(string message) {
    string* data=new string();
    int j=0;
    for (auto i=message.begin(); i !=message.end() ; ++i,++j) {
        if(*i=='\r'){
            if(*(i+3)=='\n'){
                //message body found
                *data=message.substr((unsigned long) (j + 3), message.length());
                return data;

            }else
                *i+=3;//increment the iterator to the next possible '\r'

        }
    }
    return nullptr;

}

void Client::write_to_file(const string &message_body,const string &filename) {
    ofstream output_file(filename,ios::out|ios::binary);
    output_file<<message_body;
}

int Client::get_status_code(string response) {
    return stoi((parse_string(response)->at(1)));
}




