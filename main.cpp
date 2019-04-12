#include <iostream>
#include <dirent.h>
#include "Server.h"
#include "Client.h"
int main() {
    Client c;
    c.start("POST ../Coursera_FinicalAid.txt hostname 80");
    return 0;
}