#include <string>

#include "sdl_player.h"

int main(int argc, char* argv[])
{
    std::string filepath = "resources/";
    std::string mp4      = filepath + "SampleVideo_1280x720_1mb.mp4";

    run_video_player(mp4);

    return 0;
}