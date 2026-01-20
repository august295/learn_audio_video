#ifndef __H264_H__
#define __H264_H__

#include <cstdint>
#include <string>

enum NaluType
{
    NALU_TYPE_SLICE    = 1,
    NALU_TYPE_DPA      = 2,
    NALU_TYPE_DPB      = 3,
    NALU_TYPE_DPC      = 4,
    NALU_TYPE_IDR      = 5,
    NALU_TYPE_SEI      = 6,
    NALU_TYPE_SPS      = 7,
    NALU_TYPE_PPS      = 8,
    NALU_TYPE_AUD      = 9,
    NALU_TYPE_EOSEQ    = 10,
    NALU_TYPE_EOSTREAM = 11,
    NALU_TYPE_FILL     = 12,
};

#pragma pack(1)
typedef struct H264AnnexBNalu
{
    uint8_t  start_code_len;        // 3 or 4

    uint8_t forbidden_zero_bit : 1; // F
    uint8_t nal_ref_idc        : 2; // NRI
    uint8_t nal_unit_type      : 5; // NALU type

    uint32_t date_size;             //
    uint8_t* data;                  //
} H264AnnexBNalu;
#pragma pack()

/**
 * @brief   解析H264文件
 * @param   h264                    [IN]        h264文件
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_h264_parser(const std::string& h264);

#endif