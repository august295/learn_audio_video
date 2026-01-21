#ifndef __AAC_H__
#define __AAC_H__

#include <stdint.h>
#include <string>

#pragma pack(1)
typedef struct AccAdtsHeader
{
    uint16_t syncword                  : 12; // 固定 0xFFF
    uint16_t id                        : 1;  // MPEG-4 / MPEG-2
    uint16_t layer                     : 2;  // 固定 00
    uint16_t protection_absent         : 1;  // 1=无 CRC

    uint16_t profile                   : 2;  // 配置文件
    uint16_t sampling_freq             : 4;  // MPEG-4 采样频率指数
    uint16_t private_bit               : 1;  // 私有位
    uint16_t channel_configuration     : 3;  // MPEG-4 配置
    uint16_t original                  : 1;  // 原始
    uint16_t home                      : 1;  // 家庭
    uint16_t copyright_id              : 1;  // 版权 ID 位
    uint16_t copyright_id_start        : 1;  // 版权 ID 开始
    uint16_t frame_length              : 13; // 帧长度
    uint16_t adts_buffer_fullness      : 11; // 缓冲区满度
    uint16_t number_of_raw_data_blocks : 2;  // ADTS 帧中 AAC 帧

    uint16_t crc                       : 16; // CRC
} AccAdtsHeader;
#pragma pack()

/**
 * @brief   aac文件解析
 * @param   filename                [IN]        aac 输入文件路径
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_aac_parser(const std::string& filename);

#endif