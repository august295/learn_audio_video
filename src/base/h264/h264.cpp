#include <cstdint>
#include <stdbool.h>
#include <fstream>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/color.h>

#include "h264.h"
#include "spdlog/fmt/bundled/base.h"

bool isStartCode3(const uint8_t* p)
{
    return p[0] == 0x00 && p[1] == 0x00 && p[2] == 0x01;
}

bool isStartCode4(const uint8_t* p)
{
    return p[0] == 0x00 && p[1] == 0x00 && p[2] == 0x00 && p[3] == 0x01;
}

bool GetNextNALU(std::ifstream& h264file, H264AnnexBNalu& nalu)
{
    uint8_t start_code[4] = {0};
    h264file.read((char*)start_code, 3);
    if (h264file.gcount() != 3)
    {
        return false;
    }

    if (isStartCode3(start_code))
    {
        nalu.start_code_len = 3;
    }
    else
    {
        h264file.read((char*)start_code + 3, 1);
        if (h264file.gcount() != 1 || !isStartCode4(start_code))
        {
            return false;
        }
        nalu.start_code_len = 4;
    }

    std::vector<uint8_t> buf;
    while (1)
    {
        char c = h264file.get();
        if (0 == h264file.gcount())
        {
            break;
        }

        buf.push_back(static_cast<uint8_t>(c));
        int size = static_cast<int>(buf.size());
        if (size >= 4 && isStartCode4(&buf[size - 4]))
        {
            h264file.seekg(-4, std::ios::cur);
            buf.resize(size - 4);
            break;
        }
        if (size >= 3 && isStartCode3(&buf[size - 3]))
        {
            h264file.seekg(-3, std::ios::cur);
            buf.resize(size - 3);
            break;
        }
    }

    nalu.date_size = static_cast<uint32_t>(buf.size());
    nalu.data      = new uint8_t[nalu.date_size];
    memcpy(nalu.data, buf.data(), nalu.date_size);

    if (buf.size() > 0)
    {
        nalu.forbidden_zero_bit = (buf[0] & 0x80) >> 7;
        nalu.nal_ref_idc        = (buf[0] & 0x60) >> 5;
        nalu.nal_unit_type      = buf[0] & 0x1f;
    }

    return true;
}

const char* get_nalu_type_string(int nalu_type, fmt::color& color)
{
    switch (nalu_type)
    {
    case NALU_TYPE_SLICE:    return "SLICE";
    case NALU_TYPE_DPA:      return "DPA";
    case NALU_TYPE_DPB:      return "DPB";
    case NALU_TYPE_DPC:      return "DPC";
    case NALU_TYPE_IDR:      color = fmt::color::red; return "IDR";
    case NALU_TYPE_SEI:      color = fmt::color::blue; return "SEI";
    case NALU_TYPE_SPS:      color = fmt::color::yellow; return "SPS";
    case NALU_TYPE_PPS:      color = fmt::color::orange; return "PPS";
    case NALU_TYPE_AUD:      return "AUD";
    case NALU_TYPE_EOSEQ:    return "EOSEQ";
    case NALU_TYPE_EOSTREAM: return "EOSTREAM";
    case NALU_TYPE_FILL:     return "FILL";
    default:                 return "unknown";
    }
}

int simplest_h264_parser(const std::string& h264)
{
    SPDLOG_INFO("simplest_h264_parser");

    std::ifstream h264file(h264, std::ios::in | std::ios::binary);
    if (!h264file.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", h264);
        return -1;
    }

    fmt::print("+------+------------+-----------+-----+------+-----------+--------+\n");
    fmt::print("| NUM  | Offset     | StartCode | F   | IDC  | NALU Type | Lenght |\n");
    fmt::print("+------+------------+-----------+-----+------+-----------+--------+\n");

    int            nalu_num = 0;
    int            offset   = 0;
    H264AnnexBNalu nalu     = {0};
    while (GetNextNALU(h264file, nalu))
    {
        fmt::color  color    = fmt::color::white;
        const char* type_str = get_nalu_type_string(nalu.nal_unit_type, color);
        fmt::print(fmt::fg(color), "| {:4} | 0x{:08X} | {:9} | 0b{:1b} | 0b{:02b} | {:>9} | {:6} |\n", nalu_num, offset, nalu.start_code_len, (uint8_t)nalu.forbidden_zero_bit, (uint8_t)nalu.nal_ref_idc, type_str, nalu.date_size);

        delete[] nalu.data;
        nalu_num++;
        offset += nalu.start_code_len + nalu.date_size;
    }

    h264file.close();

    return 0;
}