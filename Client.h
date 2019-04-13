//
// Created by omar_swidan on 24/03/19.
//

#ifndef SOCKET_PROGRAMMING_CLIENT_H
#define SOCKET_PROGRAMMING_CLIENT_H

#include <vector>

using namespace std;
class Client {
public:
    int start(string input);
private:
    vector<string>* parse_string(string input);
    string* make_message(string request_type,string filename,string hostname);
    string* get_data(string message);
    void write_to_file(const string &message_body,const string &filename);
    int get_status_code(string response);
    void print_addrinfo_result_linkedlist(struct addrinfo* result,string host);
    string* receive_message(int socket_descriptor);

};


#endif //SOCKET_PROGRAMMING_CLIENT_H
