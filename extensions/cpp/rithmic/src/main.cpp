#include <iostream>
#include <string>
#include <vector>
#include "RApiPlus.h"

int main(int argc, char* argv[]) {
    std::cout << "Starting Rithmic C++ Feed..." << std::endl;

    // TODO: Initialize RApiPlus
    // REngine * engine = new REngine();
    
    // Environment variables will be injected by Quanux Server
    const char* user = std::getenv("QUANUX_RITHMIC_USER");
    if (user) {
        std::cout << "User: " << user << std::endl;
    } else {
        std::cerr << "QUANUX_RITHMIC_USER not set" << std::endl;
    }

    return 0;
}
