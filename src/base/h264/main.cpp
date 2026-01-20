#include <string>

#include "h264.h"

int main(int argc, char* argv[])
{
    std::string filepath        = "resources/";
    std::string h264 = filepath + "sintel.h264";

    simplest_h264_parser(h264);

    return 0;
}