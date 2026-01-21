#include <stdbool.h>
#include <fstream>

#include <spdlog/spdlog.h>

#include "aac.h"

// clang-format off
static const int sample_rate_table[] = {
    96000, 88200, 64000, 48000, 44100, 32000,
    24000, 22050, 16000, 12000, 11025, 8000
};
// clang-format on

bool parse_adts_header(const uint8_t* adts, AccAdtsHeader& header)
{
    // syncword: 12 bits
    if (adts[0] != 0xFF || (adts[1] & 0xF0) != 0xF0)
    {
        return false;
    }

    header.id                    = (adts[1] >> 3) & 0x1;
    header.protection_absent     = (adts[1] >> 2) & 0x1;
    header.profile               = (adts[2] >> 6) & 0x3;
    header.sampling_freq         = (adts[2] >> 2) & 0xF;
    header.private_bit           = (adts[2] >> 1) & 0x1;
    header.channel_configuration = adts[2] & 0x3;
    header.original              = (adts[3] >> 7) & 0x1;
    header.home                  = (adts[3] >> 6) & 0x1;
    header.copyright_id          = (adts[3] >> 5) & 0x1;
    header.copyright_id_start    = (adts[3] >> 4) & 0x1;
    header.frame_length          = ((adts[3] & 0x3) << 11) | (adts[4] << 3) | ((adts[5] & 0xE0) >> 5);
    header.adts_buffer_fullness  = ((adts[5] & 0x1F) << 6) | (adts[6] >> 2);

    return true;
}

const char* get_profile_name(int profile)
{
    switch (profile)
    {
    case 0:  return "Main";
    case 1:  return "LC";
    case 2:  return "SSR";
    case 3:  return "LTP";
    default: return "Unknown";
    }
}

const char* get_channel_name(int ch)
{
    switch (ch)
    {
    case 0:  return "Other";
    case 1:  return "Mono";
    case 2:  return "Stereo";
    case 3:  return "3 channels";
    case 4:  return "4 channels";
    case 5:  return "5 channels";
    case 6:  return "5.1 channels";
    case 7:  return "7.1 channels";
    default: return "Unknown";
    }
}

int simplest_aac_parser(const std::string& filename)
{
    std::ifstream accFile(filename, std::ios::in | std::ios::binary);
    if (!accFile.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", filename);
        return -1;
    }

    fmt::print("+------+--------+-----+-------- +-------------+--------------+--------------+---------------+\n");
    fmt::print("| NUM  | MPEG V | PA  | Profile | Sample Rate | Channels     | Frame Length | Header Length |\n");
    fmt::print("+------+--------+-----+---------+-------------+--------------+--------------+---------------+\n");

    int           index        = 0;
    uint8_t       adts[9]      = {0};
    AccAdtsHeader accAdtsHeaer = {0};
    while (accFile.read((char*)adts, 7))
    {
        if (accFile.gcount() != 7)
        {
            SPDLOG_ERROR("Failed to read file: {}", filename);
            break;
        }
        if (!parse_adts_header(adts, accAdtsHeaer))
        {
            // 非法帧，尝试重新同步
            accFile.seekg(-6, std::ios::cur);
            continue;
        }

        // 如果有 CRC，再读 2 字节
        if (!accAdtsHeaer.protection_absent)
        {
            accFile.read((char*)adts + 7, 2);
            if (accFile.gcount() != 2)
            {
                SPDLOG_ERROR("Failed to read file: {}", filename);
                break;
            }
        }

        int header_len = accAdtsHeaer.protection_absent ? 7 : 9;
        fmt::print("| {:4} | {:6} | {:3} | {:7} | {:11} | {:12} | {:12} | {:13} |\n",
                   index,
                   accAdtsHeaer.id ? "MPEG-2" : "MPEG-4",
                   accAdtsHeaer.protection_absent ? "-" : "CRC",
                   get_profile_name(accAdtsHeaer.profile),
                   sample_rate_table[accAdtsHeaer.sampling_freq],
                   get_channel_name(accAdtsHeaer.channel_configuration),
                   (uint16_t)accAdtsHeaer.frame_length,
                   header_len);

        accFile.seekg(accAdtsHeaer.frame_length - header_len, std::ios::cur);
        index++;
    }

    accFile.close();

    return 0;
}