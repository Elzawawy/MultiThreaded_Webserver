//
// Created by Amr Elzawawy on 4/13/19.
//
#include <string>
#include <iostream>
#include "Client.h"

int main(){
    Client c;
    string command;
    cout<<"Let's do one more ! Command goes below"<<endl;
    getline (cin, command);
    c.start(command);
    return 0;
}
