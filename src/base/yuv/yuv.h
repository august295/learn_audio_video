#ifndef __YUV_H__
#define __YUV_H__

#include <string>

/**
 * @brief   分离YUV420P像素数据中的Y、U、V分量
 * @param   filename                [IN]        yuv420 输入文件路径
 * @param   width                   [IN]        yuv420 图像帧的宽度
 * @param   height                  [IN]        yuv420 图像帧的高度
 * @param   number                  [IN]        处理的帧数
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_yuv420_split(const std::string& filename, int width, int height, int number);

/**
 * @brief   分离YUV422P像素数据中的Y、U、V分量
 * @param   filename                [IN]        yuv422 输入文件路径
 * @param   width                   [IN]        yuv422 图像帧的宽度
 * @param   height                  [IN]        yuv422 图像帧的高度
 * @param   number                  [IN]        处理的帧数
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_yuv422_split(const std::string& filename, int width, int height, int number);

/**
 * @brief   分离YUV444P像素数据中的Y、U、V分量
 * @param   filename                [IN]        yuv444 输入文件路径
 * @param   width                   [IN]        yuv444 图像帧的宽度
 * @param   height                  [IN]        yuv444 图像帧的高度
 * @param   number                  [IN]        处理的帧数
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_yuv444_split(const std::string& filename, int width, int height, int number);

/**
 * @brief   将YUV420P像素数据去掉颜色（变成灰度图）
 * @param   filename                [IN]        yuv420 输入文件路径
 * @param   width                   [IN]        yuv420 图像帧的宽度
 * @param   height                  [IN]        yuv420 图像帧的高度
 * @param   number                  [IN]        处理的帧数
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_yuv420_gray(const std::string& filename, int width, int height, int number);

/**
 * @brief   将YUV420P像素数据的亮度减半
 * @param   filename                [IN]        yuv420 输入文件路径
 * @param   width                   [IN]        yuv420 图像帧的宽度
 * @param   height                  [IN]        yuv420 图像帧的高度
 * @param   number                  [IN]        处理的帧数
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_yuv420_halfy(const std::string& filename, int width, int height, int number);

/**
 * @brief   将YUV420P像素数据的周围加上边框
 * @param   filename                [IN]        yuv420 输入文件路径
 * @param   width                   [IN]        yuv420 图像帧的宽度
 * @param   height                  [IN]        yuv420 图像帧的高度
 * @param   border                  [IN]        边框宽度
 * @param   number                  [IN]        处理的帧数
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_yuv420_border(const std::string& filename, int width, int height, int border, int number);

/**
 * @brief   生成YUV420P格式的灰阶测试图
 * @param   width                   [IN]        yuv420 图像帧的宽度
 * @param   height                  [IN]        yuv420 图像帧的高度
 * @param   min                     [IN]        灰阶最小值
 * @param   max                     [IN]        灰阶最大值
 * @param   number                  [IN]        处理的帧数
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_yuv420_graybar(int width, int height, int min, int max, int number);

/**
 * @brief   计算两个YUV420P像素数据的PSNR
 * @param   filename                [IN]        yuv420 输入文件路径
 * @param   filename2               [IN]        yuv420 输入文件路径2
 * @param   width                   [IN]        yuv420 图像帧的宽度
 * @param   height                  [IN]        yuv420 图像帧的高度
 * @param   number                  [IN]        处理的帧数
 * @return  0                                   成功
 *          其他                                失败
 */
int simplest_yuv420_psnr(const std::string& filename1, const std::string& filename2, int width, int height, int number);

#endif