#include <cstdint>
#include <cstring>
#include <fstream>

#include <spdlog/spdlog.h>

#include "pcm.h"

int simplest_pcm16le_split(const std::string& pcm_16le)
{
    SPDLOG_INFO("Splitting PCM16LE file: {}", pcm_16le);
    std::ifstream pcm16le(pcm_16le, std::ios::in | std::ios::binary);
    std::ofstream left(pcm_16le + ".left", std::ios::out | std::ios::binary);
    std::ofstream right(pcm_16le + ".right", std::ios::out | std::ios::binary);
    if (!pcm16le.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", pcm_16le);
        return -1;
    }

    int  count      = 0;
    int  sampleSize = 4;
    char sample[4]  = {0};
    while (pcm16le.read(sample, sampleSize))
    {
        left.write(sample, sampleSize / 2);
        right.write(sample + sampleSize / 2, sampleSize / 2);
        count++;
    }
    SPDLOG_INFO("Sample count: {}", count);

    pcm16le.close();
    left.close();
    right.close();

    return 0;
}

int simplest_pcm16le_halfvolumeleft(const std::string& pcm_16le)
{
    SPDLOG_INFO("Half volume left channel: {}", pcm_16le);
    std::ifstream pcm16le(pcm_16le, std::ios::in | std::ios::binary);
    std::ofstream halfLeft(pcm_16le + ".halfLeft", std::ios::out | std::ios::binary);
    if (!pcm16le.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", pcm_16le);
        return -1;
    }

    int  count      = 0;
    int  sampleSize = 4;
    char sample[4]  = {0};
    while (pcm16le.read(sample, sampleSize))
    {
        *(uint16_t*)sample = *(uint16_t*)sample / 2;
        halfLeft.write(sample, sampleSize / 2);
        halfLeft.write(sample + sampleSize / 2, sampleSize / 2);
        count++;
    }
    SPDLOG_INFO("Sample count: {}", count);

    return 0;
}

int simplest_pcm16le_doublespeed(const std::string& pcm_16le)
{
    SPDLOG_INFO("Double speed: {}", pcm_16le);
    std::fstream pcm16le(pcm_16le, std::ios::in | std::ios::out | std::ios::binary);
    std::fstream doubleSpeed(pcm_16le + ".doubleSpeed", std::ios::in | std::ios::out | std::ios::binary);
    if (!pcm16le.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", pcm_16le);
        return -1;
    }

    int  count      = 0;
    int  sampleSize = 4;
    char sample[4]  = {0};
    while (pcm16le.read(sample, sampleSize))
    {
        if (count % 2 == 0)
        {
            doubleSpeed.write(sample, sampleSize);
        }
        count++;
    }
    SPDLOG_INFO("Sample count: {}", count);

    return 0;
}

int simplest_pcm16le_to_pcm8(const std::string& pcm_16le)
{
    SPDLOG_INFO("Convert PCM16LE to PCM8: {}", pcm_16le);
    std::ifstream pcm16le(pcm_16le, std::ios::in | std::ios::binary);
    std::ofstream pcm8(pcm_16le + ".pcm8", std::ios::out | std::ios::binary);
    if (!pcm16le.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", pcm_16le);
        return -1;
    }

    int  count      = 0;
    int  sampleSize = 4;
    char sample[4]  = {0};
    while (pcm16le.read(sample, sampleSize))
    {
        *(uint16_t*)sample       = *(uint16_t*)sample / (2 << 8);
        *(uint16_t*)(sample + 2) = *(uint16_t*)(sample + 2) / (2 << 8);
        pcm8.write(sample, sampleSize / 2);
        pcm8.write(sample + sampleSize / 2, sampleSize / 2);
        count++;
    }
    SPDLOG_INFO("Sample count: {}", count);

    return 0;
}

int simplest_pcm16le_cut_singlechannel(const std::string& pcm_drum, int start_num, int dur_num)
{
    SPDLOG_INFO("Cut single channel: {}", pcm_drum);
    std::ifstream pcm16le(pcm_drum, std::ios::in | std::ios::binary);
    std::ofstream cut(pcm_drum + ".cut", std::ios::out | std::ios::binary);
    if (!pcm16le.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", pcm_drum);
        return -1;
    }

    int  count      = 0;
    int  sampleSize = 2;
    char sample[2]  = {0};
    while (pcm16le.read(sample, sampleSize))
    {
        if (count >= start_num && count < start_num + dur_num)
        {
            cut.write(sample, sampleSize);
        }
        count++;
    }
    SPDLOG_INFO("Sample count: {}", count);

    return 0;
}

int simplest_pcm16le_to_wave(const std::string& pcm_16le, int channels, int sample_rate)
{
    SPDLOG_INFO("Convert PCM16LE to WAVE: {}", pcm_16le);
    std::ifstream pcm16le(pcm_16le, std::ios::in | std::ios::binary);
    std::ofstream wave(pcm_16le + ".wav", std::ios::out | std::ios::binary);
    if (!pcm16le.is_open())
    {
        SPDLOG_ERROR("Failed to open file: {}", pcm_16le);
        return -1;
    }

    // wave header
    WavHeader waveHeader;
    memcpy(waveHeader.riff, "RIFF", 4);
    waveHeader.file_size = 0;
    memcpy(waveHeader.wave, "WAVE", 4);
    memcpy(waveHeader.fmt, "fmt ", 4);
    waveHeader.fmt_size        = 16;
    waveHeader.audio_format    = 1;
    waveHeader.num_channels    = channels;
    waveHeader.sample_rate     = sample_rate;
    waveHeader.bits_per_sample = 16;
    waveHeader.byte_rate       = sample_rate * channels * (waveHeader.bits_per_sample / 8);
    waveHeader.block_align     = channels * (waveHeader.bits_per_sample / 8);
    memcpy(waveHeader.data, "data", 4);
    waveHeader.data_size = 0;

    wave.seekp(sizeof(WavHeader));
    int  sampleSize = 4;
    char sample[4]  = {0};
    while (pcm16le.read(sample, sampleSize))
    {
        wave.write(sample, sampleSize);
        waveHeader.data_size += sampleSize;
    }
    waveHeader.file_size = waveHeader.data_size + sizeof(WavHeader) - 8;
    wave.seekp(0);
    wave.write((char*)&waveHeader, sizeof(WavHeader));

    pcm16le.close();
    wave.close();

    return 0;
}