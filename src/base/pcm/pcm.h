#ifndef __PCM_H__
#define __PCM_H__

#include <string>

#pragma pack(1)
// WAV 文件头结构体
typedef struct WavHeader
{
    char     riff[4];         // "RIFF"
    uint32_t file_size;       // 文件总大小 - 8（字节）
    char     wave[4];         // "WAVE"

    char     fmt[4];          // "fmt "
    uint32_t fmt_size;        // fmt 块大小（16、18 或 40）
    uint16_t audio_format;    // 音频格式（1=PCM, 3=IEEE float, 6=ALAW, 7=MULAW...）
    uint16_t num_channels;    // 声道数（1=单声道，2=立体声）
    uint32_t sample_rate;     // 采样率（Hz）
    uint32_t byte_rate;       // 字节率 = sample_rate * num_channels * bits_per_sample/8
    uint16_t block_align;     // 块对齐 = num_channels * bits_per_sample/8
    uint16_t bits_per_sample; // 位深度（8, 16, 24, 32）

    char     data[4];         // "data"
    uint32_t data_size;       // 音频数据大小（字节）
} WavHeader;
#pragma pack()

/**
 * @brief   分离PCM16LE双声道音频采样数据的左声道和右声道
 * @param   pcm_16le                [IN]        pcm16le 输入文件路径
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_pcm16le_split(const std::string& pcm_16le);

/**
 * @brief   将PCM16LE双声道音频采样数据中左声道的音量降一半
 * @param   pcm_16le                [IN]        pcm16le 输入文件路径
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_pcm16le_halfvolumeleft(const std::string& pcm_16le);

/**
 * @brief   将PCM16LE双声道音频采样数据的声音速度提高一倍
 * @param   pcm_16le                [IN]        pcm16le 输入文件路径
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_pcm16le_doublespeed(const std::string& pcm_16le);

/**
 * @brief   将PCM16LE双声道音频采样数据转换为PCM8音频采样数据
 * @param   pcm_16le                [IN]        pcm16le 输入文件路径
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_pcm16le_to_pcm8(const std::string& pcm_16le);

/**
 * @brief   从PCM16LE单声道音频采样数据中截取一部分数据
 * @param   pcm_drum                [IN]        pcm16le 输入文件路径
 * @param   start_num               [IN]        开始位置
 * @param   dur_num                 [IN]        截取数量
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_pcm16le_cut_singlechannel(const std::string& pcm_drum, int start_num, int dur_num);

/**
 * @brief   将PCM16LE双声道音频采样数据转换为WAVE格式音频数据
 * @param   pcm_16le                [IN]        pcm16le 输入文件路径
 * @param   channels                [IN]        通道
 * @param   sample_rate             [IN]        采样率
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_pcm16le_to_wave(const std::string& pcm_16le, int channels, int sample_rate);

#endif