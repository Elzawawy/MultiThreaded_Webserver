#include <iostream>
#include <dirent.h>
#include "Server.h"
#include "Client.h"
int main() {
    Client c;
    c.start("POST ../sheet_4_pattern_recognition.html 192.168.43.153 80");
    return 0;
}