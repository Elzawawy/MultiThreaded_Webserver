#include <iostream>
#include <dirent.h>
#include "Server.h"
#include "Client.h"
int main() {
    Server c;
    c.start();
    return 0;
}