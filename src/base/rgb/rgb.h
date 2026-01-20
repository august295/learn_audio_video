#ifndef __RGB_H__
#define __RGB_H__

#include <stdint.h>
#include <string>
#include <algorithm>

#pragma pack(1)
typedef struct BmpFileHead
{
    uint16_t type;          // 2字节：文件类型 "BM"
    uint32_t imageSize;     // 4字节：文件总大小
    uint16_t reserved1;     // 2字节：保留
    uint16_t reserved2;     // 2字节：保留
    uint32_t startPosition; // 4字节：数据起始位置
} BmpFileHead;

typedef struct BmpInfoHead
{
    uint32_t length;         // 4字节：本结构体大小
    int32_t  width;          // 4字节：宽度
    int32_t  height;         // 4字节：高度
    uint16_t colorPlane;     // 2字节：颜色平面数
    uint16_t bitColor;       // 2字节：每像素位数
    uint32_t zipFormat;      // 4字节：压缩格式
    uint32_t realSize;       // 4字节：图像数据大小
    int32_t  xPels;          // 4字节：水平分辨率
    int32_t  yPels;          // 4字节：垂直分辨率
    uint32_t colorUse;       // 4字节：使用的颜色数
    uint32_t colorImportant; // 4字节：重要颜色数
} BmpInfoHead;
#pragma pack()

template <typename T>
inline T clamp_value(T value, T min_val, T max_val)
{
    return std::max(std::min(value, max_val), min_val);
}

/**
 * @brief   分离RGB24像素数据中的R、G、B分量
 * @param   filename                [IN]        rgb24 输入文件路径
 * @param   width                   [IN]        图像帧的宽度
 * @param   height                  [IN]        图像帧的高度
 * @param   number                  [IN]        处理的帧数
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_rgb24_split(const std::string& filename, int width, int height, int number);

/**
 * @brief   将RGB24格式像素数据封装为BMP图像
 * @param   filename                [IN]        rgb24 输入文件路径
 * @param   width                   [IN]        图像帧的宽度
 * @param   height                  [IN]        图像帧的高度
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_rgb24_to_bmp(const std::string& filename, int width, int height);

/**
 * @brief   将RGB24格式像素数据转换为YUV420P格式像素数据
 * @param   filename                [IN]        rgb24 输入文件路径
 * @param   width                   [IN]        图像帧的宽度
 * @param   height                  [IN]        图像帧的高度
 * @param   number                  [IN]        处理的帧数
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_rgb24_to_yuv420(const std::string& filename, int width, int height, int number);

/**
 * @brief   生成RGB24格式的彩条测试图
 * @param   width                   [IN]        图像帧的宽度
 * @param   height                  [IN]        图像帧的高度
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_rgb24_colorbar(int width, int height);

#endif