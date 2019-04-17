//
// Created by omar_swidan on 24/03/19.
//

#ifndef SOCKET_PROGRAMMING_SERVER_H
#define SOCKET_PROGRAMMING_SERVER_H


#include <sys/socket.h>
#include <iostream>
#include <error.h>
#include <netinet/in.h>
#include <cstring>
#include <vector>


class Server {

public:
    void start();

private:
    void show_welcome_message(struct sockaddr_storage *their_addr);

    void *client_handler(struct sockaddr_storage *their_addr, int client_socket);

    std::string receive_request(int client_socket);

    void send_response(int client_socket, std::string *response);

    std::string *getResponse(std::string request);

    std::vector<std::string> *parse_string(std::string input);

    std::string *readFileIfExists(std::string fileName);

    void writeToFile(const std::string &body, const std::string &fileName);

    std::string *get_data(std::string message);

    bool is_entity_body_reached(string& partial_response,int & body_starting_position);

};


#endif //SOCKET_PROGRAMMING_SERVER_H
