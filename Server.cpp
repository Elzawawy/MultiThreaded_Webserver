//
// Created by omar_swidan on 24/03/19.
//


#include <strings.h>
#include <sys/types.h>
#include <netdb.h>
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

#include "Server.h"


void *Server::client_handler(struct sockaddr_storage *their_addr, int client_socket) {
    //welcome message/
    show_welcome_message(their_addr);
    /* request */
    string request = receive_request(client_socket);
    string *response = getResponse(request);
    send_response(client_socket, response);
    close(client_socket);
    delete response;
    return nullptr;
}


void Server::start() {

    //Step1: Preparing to Launch using getaddrinfo().
    struct addrinfo hints, *results, *temp;
    struct sockaddr_storage their_addr;
    int status;
    int sock_fd, new_sockfd;
    int numBytes;
    char buf[MAXDATASIZE];
    //make sure struct hints is empty
    memset(&hints, 0, sizeof hints);
    //set your needed parameters of Server.
    hints.ai_family = AF_INET;            //IPv4
    hints.ai_socktype = SOCK_STREAM;      //TCP
    hints.ai_flags = AI_PASSIVE;          //fill in my ip for me
    //Does all kinds of good stuff for us, including DNS and service name lookups, and fills out the structs we need.
    //Params: nullptr is for IP address for local host, 80 is port number ,hints is our required params.
    //results points to a linked list of struct addrinfo.
    if ((status = getaddrinfo(nullptr, MY_PORT, &hints, &results)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }
    // loop through all the results and bind to the first we can
    for (temp = results; temp != nullptr; temp = temp->ai_next) {
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
    cout << "Hi there ! I am Listening !" << endl;
    while (true) { //Main loop
        socklen_t sin_size = sizeof their_addr;
        new_sockfd = accept(sock_fd, (struct sockaddr *) &their_addr, &sin_size);
        if (new_sockfd == -1) {
            perror("accept");
            continue;
        }
        //create a thread to handle the next client.
        thread th(&Server::client_handler, Server(), &their_addr, new_sockfd);
        //detach the thread from main thread.
        th.detach();
    }

}

void Server::show_welcome_message(struct sockaddr_storage *their_addr) {
    char str[INET_ADDRSTRLEN];
    socklen_t size = sizeof str;
    inet_ntop(their_addr->ss_family, &(((struct sockaddr_in *) their_addr)->sin_addr), str, size);
    printf("Hi you ! yes you %s\n", str);
}

std::string Server::receive_request(int client_socket) {
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
    while (true) {
        int rv = select(client_socket + 1, &readfds, nullptr, nullptr, &tv);
        if (rv == -1) {
            perror("select");
            break;
        }
        else if (rv == 0) break;
        if (FD_ISSET(client_socket, &readfds)) {
            bytes_read = static_cast<int>(recv(client_socket, &buffer[0], buffer.size(), 0));
        } else
            break;
        if (bytes_read == -1) {
            perror("recv");
            break;
        }
        if (bytes_read == 0) break;
        request.append(buffer.cbegin(), buffer.cbegin() + bytes_read);
    }
    cout<<"Bytes received: "<<request.size()<<endl;
    return request;

}

std::string *Server::getResponse(std::string request) {
    string response;
    auto *ptrToresponse = new string;
    vector<string> *requestTokens = parse_string(request);
    if (requestTokens->empty()) return new string{"Empty request"};
    string &requestMethod = requestTokens->at(0);
    if (requestMethod == "GET") {
        string &fileName = requestTokens->at(1);
        string *fileString = readFileIfExists("./ServerDirectory/" + fileName);
        if (fileString->empty()) {
            response = "HTTP/1.0 404 Not Found";
        } else {
            response = "HTTP/1.0 200 OK\r\n\r\n";
            response.append(*fileString);
        }
    } else if (requestMethod == "POST") {
        writeToFile(*get_data(request), "ServerDirectory/post_file");
        response = "HTTP/1.0 200 OK\r\n\r\n";
    }

    *ptrToresponse = response;
    return ptrToresponse;
}

vector<std::string> *Server::parse_string(std::string input) {

    istringstream iss(input);
    auto *results = new vector<string>;
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(*results));

    return results;
}

std::string *Server::readFileIfExists(std::string fileName) {
    streampos size;
    char *memblock;
    auto *fileString = new string;
    ifstream file(fileName, ios::binary | ios::in | ios::ate);
    if (file.is_open()) {
        size = file.tellg();
        memblock = new char[size];
        file.seekg(0, ios::beg);
        file.read(memblock, size);
        file.close();
        *fileString = string{memblock};
        delete[] memblock;
    }

    return fileString;
}

void Server::writeToFile(const std::string &body, const std::string &fileName) {
    std::ofstream file(fileName, ios::out | ios::binary | ios::trunc);
    file << body;
}

std::string *Server::get_data(std::string message) {
    string *data = new string();
    int j = 0;
    for (auto i = message.begin(); i != message.end(); ++i, ++j) {
        if (*i == '\r') {
            if (*(i + 3) == '\n') {
                //message body found
                *data = message.substr((unsigned long) (j + 3), message.length());
                return data;

            } else
                *i += 3;//increment the iterator to the next possible '\r'

        }
    }
    return nullptr;
}

void Server::send_response(int client_socket, std::string *response) {
    auto message = (*response).c_str();
    size_t len = response->length();
    ssize_t num_bytes;
    if ((num_bytes = send(client_socket, message, len, 0)) == -1)
        perror("send");
    cout<<"Bytes Sent: "<<num_bytes<<endl;
}


bool Server::is_entity_body_reached(string& partial_response,int &body_starting_position) {
    static int stopping_delimiters_count = 0;
    int j = 0;
    for (auto i = partial_response.begin(); i != partial_response.end(); ++i, ++j) {
        if (*i == '\r' || *i == '\n') {
            stopping_delimiters_count++;
            if(stopping_delimiters_count==4){
                body_starting_position=j+1;
                return true;
            }
        }else
            stopping_delimiters_count=0;
    }
    return false;
}
