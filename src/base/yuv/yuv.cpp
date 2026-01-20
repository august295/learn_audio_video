#include <cmath>
#include <fstream>

#include <spdlog/spdlog.h>

#include "yuv.h"

int simplest_yuv420_split(const std::string& filename, int width, int height, int number)
{
    std::ifstream iFile(filename, std::ios::in | std::ios::binary);
    std::ofstream yFile(filename + ".y", std::ios::out | std::ios::binary);
    std::ofstream uFile(filename + ".u", std::ios::out | std::ios::binary);
    std::ofstream vFile(filename + ".v", std::ios::out | std::ios::binary);
    if (!iFile.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", filename);
        return -1;
    }

    int   frameSize = width * height * 3 / 2; // YUV420P每帧大小
    int   ySize     = width * height;         // Y分量大小
    char* frame     = new char[frameSize];

    for (int i = 0; i < number; i++)
    {
        iFile.read(frame, frameSize);
        yFile.write(frame, ySize);
        uFile.write(frame + ySize, ySize / 4);
        vFile.write(frame + ySize + ySize / 4, ySize / 4);
    }

    delete[] frame;
    iFile.close();
    yFile.close();
    uFile.close();
    vFile.close();

    return 0;
}

int simplest_yuv422_split(const std::string& filename, int width, int height, int number)
{
    std::ifstream iFile(filename, std::ios::in | std::ios::binary);
    std::ofstream yFile(filename + ".y", std::ios::out | std::ios::binary);
    std::ofstream uFile(filename + ".u", std::ios::out | std::ios::binary);
    std::ofstream vFile(filename + ".v", std::ios::out | std::ios::binary);
    if (!iFile.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", filename);
        return -1;
    }

    int   frameSize = width * height * 2; // YUV422P每帧大小
    int   ySize     = width * height;     // Y分量大小
    char* frame     = new char[frameSize];

    for (int i = 0; i < number; i++)
    {
        iFile.read(frame, frameSize);
        yFile.write(frame, ySize);
        uFile.write(frame + ySize, ySize / 2);
        vFile.write(frame + ySize + ySize / 2, ySize / 2);
    }

    delete[] frame;
    iFile.close();
    yFile.close();
    uFile.close();
    vFile.close();

    return 0;
}

int simplest_yuv444_split(const std::string& filename, int width, int height, int number)
{
    std::ifstream iFile(filename, std::ios::in | std::ios::binary);
    std::ofstream yFile(filename + ".y", std::ios::out | std::ios::binary);
    std::ofstream uFile(filename + ".u", std::ios::out | std::ios::binary);
    std::ofstream vFile(filename + ".v", std::ios::out | std::ios::binary);
    if (!iFile.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", filename);
        return -1;
    }

    int   frameSize = width * height * 3; // YUV444P每帧大小
    int   ySize     = width * height;     // Y分量大小
    char* frame     = new char[frameSize];

    for (int i = 0; i < number; i++)
    {
        iFile.read(frame, frameSize);
        yFile.write(frame, ySize);
        uFile.write(frame + ySize, ySize);
        vFile.write(frame + ySize + ySize, ySize);
    }

    delete[] frame;
    iFile.close();
    yFile.close();
    uFile.close();
    vFile.close();

    return 0;
}

int simplest_yuv420_gray(const std::string& filename, int width, int height, int number)
{
    std::ifstream iFile(filename, std::ios::in | std::ios::binary);
    std::ofstream oFile(filename + ".gray", std::ios::out | std::ios::binary);
    if (!iFile.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", filename);
        return -1;
    }

    int   frameSize = width * height * 3 / 2; // YUV420P每帧大小
    char* frame     = new char[frameSize];

    for (int i = 0; i < number; i++)
    {
        iFile.read(frame, frameSize);
        // u和v分量设为128
        memset(frame + width * height, 128, width * height / 2);
        oFile.write(frame, width * height);
    }

    delete[] frame;
    iFile.close();
    oFile.close();

    return 0;
}

int simplest_yuv420_halfy(const std::string& filename, int width, int height, int number)
{
    std::ifstream iFile(filename, std::ios::in | std::ios::binary);
    std::ofstream oFile(filename + ".halfy", std::ios::out | std::ios::binary);
    if (!iFile.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", filename);
        return -1;
    }

    int   frameSize = width * height * 3 / 2; // YUV420P每帧大小
    char* frame     = new char[frameSize];

    for (int i = 0; i < number; i++)
    {
        iFile.read(frame, frameSize);
        for (int j = 0; j < width * height; j++)
        {
            int y    = static_cast<unsigned char>(frame[j]);
            frame[j] = static_cast<char>(y / 2);
        }
        oFile.write(frame, width * height);
    }

    delete[] frame;
    iFile.close();
    oFile.close();

    return 0;
}

int simplest_yuv420_border(const std::string& filename, int width, int height, int border, int number)
{
    std::ifstream iFile(filename, std::ios::in | std::ios::binary);
    std::ofstream oFile(filename + ".border", std::ios::out | std::ios::binary);
    if (!iFile.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", filename);
        return -1;
    }

    int   frameSize = width * height * 3 / 2; // YUV420P每帧大小
    char* frame     = new char[frameSize];

    for (int i = 0; i < number; i++)
    {
        iFile.read(frame, frameSize);
        // 添加边框
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (x < border || x >= width - border || y < border || y >= height - border)
                {
                    frame[y * width + x] = 0; // Y分量设为0（黑色）
                }
            }
        }
        oFile.write(frame, frameSize);
    }

    delete[] frame;
    iFile.close();
    oFile.close();

    return 0;
}

int simplest_yuv420_graybar(int width, int height, int min, int max, int number)
{
    std::string filename = fmt::format("graybar_{}x{}.yuv", width, height);
    std::ofstream oFile(filename, std::ios::out | std::ios::binary);
    if (!oFile.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", filename);
        return -1;
    }

    int   frameSize = width * height * 3 / 2; // YUV420P每帧大小
    char* frame     = new char[frameSize];

    for (int i = 0; i < number; i++)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int gray = min + (max - min) * x / width;
                frame[y * width + x] = static_cast<char>(gray);
            }
        }
        oFile.write(frame, frameSize);
    }

    delete[] frame;
    oFile.close();

    return 0;
}

int simplest_yuv420_psnr(const std::string& filename1, const std::string& filename2, int width, int height, int number)
{
    std::ifstream iFile1(filename1, std::ios::in | std::ios::binary);
    std::ifstream iFile2(filename2, std::ios::in | std::ios::binary);
    if (!iFile1.is_open() || !iFile2.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {} or {}", filename1, filename2);
        return -1;
    }

    int   frameSize = width * height * 3 / 2; // YUV420P每帧大小
    char* frame1    = new char[frameSize];
    char* frame2    = new char[frameSize];

    for (int i = 0; i < number; i++)
    {
        iFile1.read(frame1, frameSize);
        iFile2.read(frame2, frameSize);

        double mse = 0.0;
        for (int j = 0; j < width * height; j++)
        {
            int diff = static_cast<int>(frame1[j]) - static_cast<int>(frame2[j]);
            mse += std::pow(diff, 2);
        }
        mse /= (width * height);
        double psnr = 10 * log10((255.0 * 255.0) / mse);
        SPDLOG_INFO("Frame {}: PSNR = {:.2f} dB", i, psnr);
    }

    delete[] frame1;
    delete[] frame2;
    iFile1.close();
    iFile2.close();

    return 0;
}