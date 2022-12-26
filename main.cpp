#include <iostream>
#include "config.h"

int main(int, char**) 
{
    try {
        Configuration config = getConfiguration();
    }
    catch(std::exception& e)
    {
        std::cerr << "There was a problem: " << e.what() << std::endl;
    }
}