#include <fstream>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/color.h>

#include "flv.h"

const char* get_flv_tag_type_name(FLVTagType tagType)
{
    switch (tagType)
    {
    case FLV_TAG_TYPE_AUDIO:
        return "audio";
    case FLV_TAG_TYPE_VIDEO:
        return "video";
    case FLV_TAG_TYPE_SCRIPT_DATA:
        return "script";
    default:
        return "unknown";
    }
}

const char* SOUND_FORMAT[] = {
    "Linear PCM, platform endian",
    "ADPCM",
    "MP3",
    "Linear PCM, little endian",
    "Nellymoser 16-kHz mono",
    "Nellymoser 8-kHz mono",
    "Nellymoser",
    "G.711 A-law logarithmic PCM",
    "G.711 mu-law logarithmic PCM",
    "reserved",
    "AAC",
    "Speex",
    "MP3 8-Khz",
    "Device-specific sound",
};
int         SOUND_RATE[] = {5500, 11000, 22000, 44000};
int         SOUND_SIZE[] = {8, 16};
const char* SOUND_TYPE[] = {"Mono", "Stereo"};

const char* FRAME_TYPE[] = {
    "none",
    "keyframe",
    "inter frame",
    "disposable inter frame",
    "generated keyframe",
    "video info/command frame",
};
const char* CODEC_ID[] = {
    "none",
    "JPEG",
    "Sorenson H263",
    "Screen video",
    "On2 VP6",
    "On2 VP6 with alpha channel",
    "Screen video version 2",
    "AVC",
};

const char* SCRIPT_TYPE[] = {
    "Number",
    "Boolean",
    "String",
    "Object",
    "MovieClip",
    "Null",
    "Undefined",
    "Reference",
    "ECMA array",
    "Strict array",
    "Date",
    "Long string",
};

int simplest_flv_parser(const std::string& flv)
{
    std::ifstream flvFile(flv, std::ios::binary);
    if (!flvFile.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", flv);
        return -1;
    }

    uint8_t header[9] = {0};
    flvFile.read((char*)header, 9);
    if (flvFile.gcount() != 9)
    {
        SPDLOG_ERROR("Failed to read file: {}", flv);
        return -1;
    }
    if (memcmp(header, "FLV", 3) != 0)
    {
        SPDLOG_ERROR("Not a flv file: {}", flv);
        return -1;
    }

    FLVHeader flvHeader = {0};
    memcpy(flvHeader.signature, header, 3);
    flvHeader.version     = header[3];
    flvHeader.flags_audio = (header[4] >> 2) & 0x01;
    flvHeader.flags_video = header[4] & 0x01;
    flvHeader.data_offset = bytes_to_int_big_endian<uint32_t, 4>(header + 5);

    fmt::print("signature  : {}\n", std::string((char*)flvHeader.signature, 3));
    fmt::print("version    : {}\n", flvHeader.version);
    fmt::print("flags_audio: {}\n", (uint8_t)flvHeader.flags_audio);
    fmt::print("flags_video: {}\n", (uint8_t)flvHeader.flags_video);
    fmt::print("data_offset: {}\n", flvHeader.data_offset);

    // 跳过header
    // flvFile.seekg(flvHeader.data_offset, std::ios::beg);

    fmt::print("+------+----------+-----------+-----------+-----------+\n");
    fmt::print("| NUM  | Tag Type | Data Size | Timestamp | Stream ID | Tag Data\n");
    fmt::print("+------+----------+-----------+-----------+-----------+\n");
    fmt::color   color           = fmt::color::white;
    size_t       index           = 0;
    size_t       previousTagSize = 0;
    uint8_t      previousTag[4]  = {0};
    uint8_t      tagHeader[11]   = {0};
    FLVTagHeader flvTagHeader    = {0};
    do
    {
        // 上一个 tag 长度
        flvFile.read((char*)previousTag, 4);
        previousTagSize = bytes_to_int_big_endian<uint32_t, 4>(previousTag);
        if (flvFile.gcount() != 4)
        {
            SPDLOG_ERROR("Failed to read file: {}", flv);
            break;
        }

        // 单个 tag
        flvFile.read((char*)tagHeader, 11);
        if (flvFile.gcount() != 11)
        {
            SPDLOG_ERROR("Failed to read file: {}", flv);
            break;
        }
        flvTagHeader.tag_type           = tagHeader[0] & 0x1f;
        flvTagHeader.data_size          = bytes_to_int_big_endian<uint32_t, 3>(tagHeader + 1);
        flvTagHeader.timestamp          = bytes_to_int_big_endian<uint32_t, 3>(tagHeader + 4);
        flvTagHeader.timestamp_extended = tagHeader[7];
        flvTagHeader.stream_id          = bytes_to_int_big_endian<uint32_t, 3>(tagHeader + 8);

        std::string tagDataStr;
        switch (flvTagHeader.tag_type)
        {
        case FLV_TAG_TYPE_AUDIO: {
            uint8_t tagData[2] = {0};
            flvFile.read((char*)tagData, 2);
            if (flvFile.gcount() != 2)
            {
                SPDLOG_ERROR("Failed to read file: {}", flv);
                break;
            }
            TagDataAudio tagDataAudio    = {0};
            tagDataAudio.sound_format    = (tagData[0] >> 4) & 0x0f;
            tagDataAudio.sound_rate      = (tagData[0] >> 2) & 0x03;
            tagDataAudio.sound_size      = (tagData[0] >> 1) & 0x01;
            tagDataAudio.sound_type      = tagData[0] & 0x01;
            tagDataAudio.aac_packet_type = tagData[1];

            tagDataStr = fmt::format("Sound Format: {}, Sound Rate: {}, Sound Size: {}, Sound Type: {}, AAC Packet Type: {}",
                                     SOUND_FORMAT[tagDataAudio.sound_format],
                                     SOUND_RATE[tagDataAudio.sound_rate],
                                     SOUND_SIZE[tagDataAudio.sound_size],
                                     SOUND_TYPE[tagDataAudio.sound_type],
                                     (uint8_t)tagDataAudio.aac_packet_type);
            flvFile.seekg(-2, std::ios::cur);
            color = fmt::color::light_blue;
        }
        break;
        case FLV_TAG_TYPE_VIDEO: {
            uint8_t tagData[1] = {0};
            flvFile.read((char*)tagData, 1);
            if (flvFile.gcount() != 1)
            {
                SPDLOG_ERROR("Failed to read file: {}", flv);
                break;
            }

            TagDataVideo tagDataVideo = {0};
            tagDataVideo.frame_type   = (tagData[0] >> 4) & 0x0f;
            tagDataVideo.codec_id     = (tagData[0]) & 0x0f;

            tagDataStr = fmt::format("Frame Type: {}, Codec ID: {}",
                                     FRAME_TYPE[tagDataVideo.frame_type],
                                     CODEC_ID[tagDataVideo.codec_id]);
            flvFile.seekg(-1, std::ios::cur);
            color = fmt::color::yellow;
        }
        break;
        case FLV_TAG_TYPE_SCRIPT_DATA: {
            uint8_t tagData[1] = {0};
            flvFile.read((char*)tagData, 1);
            if (flvFile.gcount() != 1)
            {
                SPDLOG_ERROR("Failed to read file: {}", flv);
                break;
            }
            tagDataStr = fmt::format("Script Data: {}", SCRIPT_TYPE[tagData[0]]);
            flvFile.seekg(-1, std::ios::cur);
            color = fmt::color::tan;
        }
        break;
        default:
            break;
        }

        fmt::print(fmt::fg(color),
                   "| {:4} | {:8} | {:9} | {:9} | {:9} | {:50}\n",
                   index,
                   get_flv_tag_type_name((FLVTagType)flvTagHeader.tag_type),
                   flvTagHeader.data_size,
                   flvTagHeader.timestamp,
                   flvTagHeader.stream_id,
                   tagDataStr);
        flvFile.seekg(flvTagHeader.data_size, std::ios::cur);
        index++;
    } while (true);

    flvFile.close();

    return 0;
}