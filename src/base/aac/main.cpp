#include <string>

#include "aac.h"

int main(int argc, char* argv[])
{
    std::string filepath = "resources/";
    std::string aac      = filepath + "nocturne.aac";

    simplest_aac_parser(aac.c_str());

    return 0;
}