#include "simulator/CommandLineInterface.hpp"

#include <iostream>

int main() {
    project31::CommandLineInterface commandLine(std::cin, std::cout);
    return commandLine.run();
}
