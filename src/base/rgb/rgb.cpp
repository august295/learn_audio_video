#include <cstdint>
#include <fstream>

#include <spdlog/spdlog.h>

#include "rgb.h"

int simplest_rgb24_split(const std::string& filename, int width, int height, int number)
{
    std::ifstream iFile(filename, std::ios::in | std::ios::binary);
    std::ofstream rFile(filename + ".r", std::ios::out | std::ios::binary);
    std::ofstream gFile(filename + ".g", std::ios::out | std::ios::binary);
    std::ofstream bFile(filename + ".b", std::ios::out | std::ios::binary);
    if (!iFile.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", filename);
        return -1;
    }

    int   frameSize = width * height * 3; // RGB24每帧大小
    char* frame     = new char[frameSize];

    for (int i = 0; i < number; i++)
    {
        iFile.read(frame, frameSize);
        for (int j = 0; j < frameSize; j += 3)
        {
            rFile.write(frame + j, 1);
            gFile.write(frame + j + 1, 1);
            bFile.write(frame + j + 2, 1);
        }
    }

    delete[] frame;
    iFile.close();
    rFile.close();
    gFile.close();
    bFile.close();

    return 0;
}

int simplest_rgb24_to_bmp(const std::string& filename, int width, int height)
{
    std::ifstream iFile(filename, std::ios::in | std::ios::binary);
    std::ofstream oFile(filename + ".bmp", std::ios::out | std::ios::binary);
    if (!iFile.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", filename);
        return -1;
    }

    int   frameSize = width * height * 3; // RGB24每帧大小
    char* frame     = new char[frameSize];

    iFile.read(frame, frameSize);

    BmpFileHead bmpFileHead   = {0};
    bmpFileHead.type          = 0x4D42; // "BM"
    bmpFileHead.imageSize     = frameSize + sizeof(BmpFileHead);
    bmpFileHead.startPosition = sizeof(BmpFileHead);

    BmpInfoHead bmpInfoHead = {0};
    bmpInfoHead.length      = sizeof(BmpInfoHead);
    bmpInfoHead.width       = width;
    bmpInfoHead.height      = -height; // 负数表示上下翻转
    bmpInfoHead.colorPlane  = 1;
    bmpInfoHead.bitColor    = 24;
    bmpInfoHead.realSize    = frameSize;

    oFile.write(reinterpret_cast<char*>(&bmpFileHead), sizeof(BmpFileHead));
    oFile.write(reinterpret_cast<char*>(&bmpInfoHead), sizeof(BmpInfoHead));

    // BMP save R1|G1|B1,R2|G2|B2 as B1|G1|R1,B2|G2|R2
    // It saves pixel data in Little Endian
    // So we change 'R' and 'B'
    for (int i = 0; i < frameSize; i += 3)
    {
        oFile.write(frame + i + 2, 1);
        oFile.write(frame + i + 1, 1);
        oFile.write(frame + i, 1);
    }

    delete[] frame;
    iFile.close();
    oFile.close();

    return 0;
}

/**
 * RGB 转 YUV
 *  Y = 0.299R + 0.587G + 0.114B
 *  U= -0.147R - 0.289G + 0.436B
 *  V = 0.615R - 0.515G - 0.100B
 * YUV 转 RGB
 *  R = Y + 1.14V
 *  G = Y - 0.39U - 0.58V
 *  B = Y + 2.03U
 */
int rgb24_to_yuv420(const char* rgb24, int width, int height, char* yuv420)
{
    char* y = yuv420;
    char* u = yuv420 + width * height;
    char* v = yuv420 + width * height * 5 / 4;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int     pos  = (i * width + j) * 3;
            uint8_t r    = rgb24[pos];
            uint8_t g    = rgb24[pos + 1];
            uint8_t b    = rgb24[pos + 2];
            uint8_t yVal = (uint8_t)((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
            uint8_t uVal = (uint8_t)((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
            uint8_t vVal = (uint8_t)((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;

            *(y++) = clamp_value<uint8_t>(yVal, 0, 255);
            if ((i & 1) == 0 && (j & 1) == 0)
            {
                *(u++) = clamp_value<uint8_t>(uVal, 0, 255);
            }
            else
            {
                if ((j & 1) == 0)
                {
                    *(v++) = clamp_value<uint8_t>(vVal, 0, 255);
                }
            }
        }
    }

    return 0;
}

int simplest_rgb24_to_yuv420(const std::string& filename, int width, int height, int number)
{
    std::ifstream iFile(filename, std::ios::in | std::ios::binary);
    std::ofstream oFile(filename + ".yuv", std::ios::out | std::ios::binary);
    if (!iFile.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", filename);
        return -1;
    }

    int   frameSize = width * height * 3; // RGB24每帧大小
    char* frame     = new char[frameSize];
    char* yuv420    = new char[frameSize / 2];

    for (int i = 0; i < number; i++)
    {
        iFile.read(frame, frameSize);
        rgb24_to_yuv420(frame, width, height, yuv420);
        oFile.write(yuv420, frameSize / 2);
    }

    delete[] frame;
    delete[] yuv420;
    iFile.close();
    oFile.close();

    return 0;
}

int simplest_rgb24_colorbar(int width, int height)
{
    std::string   filename = fmt::format("colorbar_{}x{}.rgb", width, height);
    std::ofstream oFile(filename, std::ios::out | std::ios::binary);
    if (!oFile.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", filename);
        return -1;
    }

    int   frameSize = width * height * 3; // RGB24每帧大小
    char* frame     = new char[frameSize];
    int   barwidth  = width / 8;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int r = 0, g = 0, b = 0;
            if (j < barwidth)
            {
                r = 0xff;
            }
            else if (j < barwidth * 2)
            {
                g = 0xff;
            }
            else if (j < barwidth * 3)
            {
                b = 0xff;
            }
            else if (j < barwidth * 4)
            {
                r = 0xff;
                g = 0xff;
            }
            else if (j < barwidth * 5)
            {
                g = 0xff;
                b = 0xff;
            }
            else if (j < barwidth * 6)
            {
                r = 0xff;
                b = 0xff;
            }
            else if (j < barwidth * 7)
            {
                r = 0xff;
                g = 0xff;
                b = 0xff;
            }
            frame[(i * width + j) * 3 + 0] = r;
            frame[(i * width + j) * 3 + 1] = g;
            frame[(i * width + j) * 3 + 2] = b;
        }
    }

    oFile.write(frame, frameSize);
    delete[] frame;
    oFile.close();

    return 0;
}