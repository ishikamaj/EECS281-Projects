// PROJECT IDENTIFIER: AD48FB4835AF347EB0CA8009E24C3B13F8519882

#include <iostream>
#include "starwars.h"

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    Options userOptions = getOptions(argc, argv);
    Input userInput = getInput();
    galacticWarfare(userOptions, userInput);
    return 0;
}