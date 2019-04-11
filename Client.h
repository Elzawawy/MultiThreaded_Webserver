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

};


#endif //SOCKET_PROGRAMMING_CLIENT_H
