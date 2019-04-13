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
#include <vector>
#include <iterator>
#include <fstream>

#define MAXDATASIZE 256000 // max number of bytes we can get at once
#define MY_PORT "80"
#define BACK_LOG 10
#pragma clang diagnostic push
#pragma clang diagnostic pop
#pragma clang diagnostic ignored "-Wmissing-noreturn"
using namespace std;
string* getResponse(string request);

void* clientHandler(struct sockaddr_storage* their_addr,int client_socket)
{
//    char string[INET_ADDRSTRLEN];
//    socklen_t size = sizeof string;
//    inet_ntop(their_addr->ss_family, &(((struct sockaddr_in*)((struct sockaddr *)&their_addr)))->sin_addr),string,size);
//    printf("server: got connection from %s\n", string);
    cout<<"Connected to a client !"<<endl;
    /* request */
    string buffer;
    fd_set readfds;
    struct timeval tv;
    string request;
    buffer.clear();
    buffer.resize(MAXDATASIZE);
    int bytes_read;
    // clear the set ahead of time
    FD_ZERO(&readfds);
    // add our descriptors to the set
    FD_SET(client_socket, &readfds);
    // wait until either socket has data ready to be recv()d (timeout 10.5 secs)
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    while(true)
    {
        int rv = select(client_socket+1, &readfds, nullptr, nullptr, &tv);
        cout<<"I am unblocked bitch !"<<endl;
        cout<<rv<<endl;
        if(rv == -1) {perror("select"); break;}
        else if(rv == 0) break;
        if(FD_ISSET(client_socket,&readfds)) {
            bytes_read = static_cast<int>(recv(client_socket, &buffer[0],buffer.size(), 0));
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
        request.append(buffer.cbegin(),buffer.cbegin()+bytes_read);
        cout<<"Buffer"<<buffer.size()<<endl;
        cout<<"sizeeeee "<<request.size()<<endl;
    }
    string* response = getResponse(request);
    auto message = (*response).c_str();
    size_t len = response->length();
    ssize_t num_bytes;
    if ((num_bytes =send(client_socket,message,len, 0)) == -1)
        perror("send");
    cout<<"send"<<num_bytes<<endl;
    close(client_socket);
    delete response;
    return nullptr;
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
        th.detach();
    }

}
vector<string>* parse_string(string input)
{


    istringstream iss(input);
    auto *results = new vector<string>;
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(*results));

    return results;

}

string* readFileIfExists(string fileName)
{
    streampos size;
    char * memblock;
    auto *fileString = new string;
    ifstream file(fileName,ios::binary|ios::in|ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        memblock = new char [size];
        file.seekg (0, ios::beg);
        file.read (memblock, size);
        file.close();
        *fileString = string{memblock};
        delete [] memblock;
    }

    return fileString;
}

void writeToFile(const string &body, const string &fileName)
{
    std::ofstream file(fileName,ios::out|ios::binary| ios::trunc);
    file << body;
}
string* get_data(string message) {
    string* data=new string();
    int j =0;
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

string* getResponse(string request)
{
    string response;
    auto * ptrToresponse = new string;
    vector<string>* requestTokens = parse_string(request);
    if(requestTokens->empty()) return new string{"Empty request"};
    string &requestMethod = requestTokens->at(0);
    if(requestMethod == "GET")
    {
        string &fileName = requestTokens->at(1);
        string* fileString = readFileIfExists("./ServerDirectory/"+fileName);
        if(fileString->empty())
        {
            response = "HTTP/1.0 404 Not Found";
        }
        else
        {
            response = "HTTP/1.0 200 OK\r\n\r\n";
            response.append(*fileString);
        }
    }
    else if(requestMethod == "POST")
    {
        writeToFile(*get_data(request),"ServerDirectory/post_file");
        response = "HTTP/1.0 200 OK\r\n\r\n";
    }

    *ptrToresponse = response;
    return ptrToresponse;

}



