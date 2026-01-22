#ifndef __FLV_H__
#define __FLV_H__

#include <stdint.h>
#include <string>

enum FLVTagType
{
    FLV_TAG_TYPE_AUDIO       = 0x08,
    FLV_TAG_TYPE_VIDEO       = 0x09,
    FLV_TAG_TYPE_SCRIPT_DATA = 0x12,
};

#pragma pack(1)
typedef struct FLVHeader
{
    uint8_t  signature[3];        // FLV
    uint8_t  version;             // 版本
    uint8_t  flags_reserved  : 5; // 保留
    uint8_t  flags_audio     : 1; // 1表示有音频
    uint8_t  flags_reserved2 : 1; // 保留
    uint8_t  flags_video     : 1; // 1表示有视频
    uint32_t data_offset;         // header长度
} FLVHeader;

typedef struct FLVTagHeader
{
    uint8_t  reserved : 2;       // 保留
    uint8_t  filter   : 1;       // 0
    uint8_t  tag_type : 5;       // tag类型：0x08表示audio；0x09表示video；0x12表示script data
    uint32_t data_size;          // tag Data的大小
    uint32_t timestamp;          // 相对于第一个tag的时间戳
    uint8_t  timestamp_extended; // 时间戳的扩展字段，当 Timestamp 3个字节不够时，会启用这个字段，代表高8位
    uint32_t stream_id;          // 总是0
} FLVTag;

typedef struct TagDataAudio
{
    uint8_t  sound_format : 4; // 音频格式
    uint8_t  sound_rate   : 2; // 音频采样率
    uint8_t  sound_size   : 1; // 音频采样位数
    uint8_t  sound_type   : 1; // 音频类型
    uint8_t  aac_packet_type;  // aac包类型
    uint8_t* sound_data;       // 音频数据
} TagDataAudio;

typedef struct TagDataVideo
{
    uint8_t  frame_type : 4; // 视频格式
    uint8_t  codec_id   : 4; // 视频编码id
    uint8_t* video_data;     // 视频数据
} TagDataVideo;
#pragma pack()

template <typename T, size_t N>
T bytes_to_int_big_endian(const uint8_t* p)
{
    static_assert(std::is_integral_v<T>, "T must be an integral type");
    static_assert(sizeof(T) >= N, "T is too small");

    T r = 0;
    for (size_t i = 0; i < N; ++i)
    {
        r |= static_cast<T>(p[i]) << ((N - 1 - i) * 8);
    }
    return r;
}

template <typename T, size_t N>
T bytes_to_int_little_endian(const uint8_t* p)
{
    static_assert(std::is_integral_v<T>, "T must be an integral type");
    static_assert(sizeof(T) >= N, "T is too small");

    T r = 0;
    for (size_t i = 0; i < N; ++i)
    {
        r |= static_cast<T>(p[i]) << (i * 8);
    }
    return r;
}

/**
 * @brief   flv解析
 * @param   flv                     [IN]        flv文件
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_flv_parser(const std::string& flv);

#endif