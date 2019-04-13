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
#define MAXDATASIZE 256000
#define STD_INPUT_SIZE 4
int Client::start(string input){

    const vector<string>* tokens=parse_string(input);//parse the input string and tokenize it
    const string *message=make_message(tokens->at(0),tokens->at(1),tokens->at(2));//make a message string by appending the header to the body if POST
    const string message_type=(tokens->at(0));
    int status;
    const char *host =(tokens->at(2)).c_str();
    const string port_number=(tokens->size())== STD_INPUT_SIZE?(tokens->at(3)):"80";
    struct addrinfo hints,*res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;//ipv4
    hints.ai_socktype = SOCK_STREAM;//TCP

    if ((status = getaddrinfo(host, "80", &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    print_addrinfo_result_linkedlist(res,host);//print the contents of the res linked list

    //getting socket descriptor for upcoming system calls
    int socket_descriptor = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    //trying to connect to the server
    if (connect(socket_descriptor, res->ai_addr, res->ai_addrlen) == -1) {
        close(socket_descriptor);
        perror("client: connect");
    }

    //send request message
    int len, bytes_sent;

    len = (int) message->length();
    cout<<"message length"<<len<<endl;
    bytes_sent = (int) send(socket_descriptor, message->c_str(), (size_t) len, 0);

    cout<<"sent"<<bytes_sent<<endl;
//
    string* response=receive_message(socket_descriptor);


    auto message_body = new string;

   if(get_status_code(*response)==200) {
       if (message_type == "GET")
           message_body = (get_data(*response));
       else
           *message_body = string{"message posted"};
       write_to_file(*(message_body),"get_file");
   } else cout<<"request error"<<endl;


    close(socket_descriptor);
    freeaddrinfo(res); // free the linked list
    delete(response);
    return 0;

}


string *Client::make_message(string request_type,string filename,string hostname) {

    string *message=new string();
    message->append(request_type+" "+filename+" "+"HTTP/1.0"+"\r\n");

    message->append("\r\n");
    if(request_type=="POST"){

        streampos size;
//        char * memblock;
        string memblock;
        ifstream post_file(filename,ios::binary|ios::in|ios::ate);
        if (post_file.is_open())
        {

            size = post_file.tellg();

//            memblock = new char [size];
            memblock.resize((unsigned long) size);
            post_file.seekg (0, ios::beg);
            post_file.read (&memblock[0], memblock.size());

            cout<<"eof "<<post_file.eof()<<endl;
            cout<<"filesize "<<size<<endl;

            post_file.close();

            message->append(memblock);

            cout<<message->size()<<endl;
//            delete[] memblock;


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
    ofstream output_file(filename,ios::out|ios::binary|ios::trunc);
    output_file<<message_body;
}

int Client::get_status_code(string response) {
    if(response.size()<2)
        return 0;

    return stoi((parse_string(response)->at(1)));
}

void Client::print_addrinfo_result_linkedlist(addrinfo* result,string host) {
    struct addrinfo *p=result;
//Printing the contents of the linked list
    char ipstr[INET6_ADDRSTRLEN];

    cout<<"IP addresses for "<<host<<"\n";



    for(;p != NULL; p = p->ai_next) {

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
}

string* Client::receive_message(int socket_descriptor) {
    //recieve message with select
    string buffer;
    fd_set readfds;
    struct timeval tv;
    string* response=new string;
    buffer.clear();
    buffer.resize(MAXDATASIZE);
    int bytes_read;
    // clear the set ahead of time
    FD_ZERO(&readfds);
    // add our descriptors to the set
    FD_SET(socket_descriptor, &readfds);
    // wait until either socket has data ready to be recv()d (timeout 10.5 secs)
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    while(true)
    {
        int rv = select(socket_descriptor+1, &readfds, nullptr, nullptr, &tv);
        cout<<"I am unblocked bitch !"<<endl;
        cout<<rv<<endl;
        if(rv == -1) {perror("select"); break;}
        else if(rv == 0) break;
        if(FD_ISSET(socket_descriptor,&readfds)) {
            bytes_read = static_cast<int>(recv(socket_descriptor, &buffer[0],buffer.size(), 0));
        }
        else
            break;
        if(bytes_read == -1)
        {
            perror("recv");
            break;
        }
        cout<<"rec"<<bytes_read<<endl;
        if(bytes_read == 0) break;
        response->append(buffer.cbegin(),buffer.cbegin()+bytes_read);
        cout<<"Buffer"<<buffer.size()<<endl;
        cout<<"sizeeeee "<<response->size()<<endl;
    }
    return response;
}




