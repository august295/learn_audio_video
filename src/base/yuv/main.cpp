#include <string>

#include "yuv.h"

int main(int argc, char* argv[])
{
    std::string filepath        = "resources/";
    std::string yuv420p         = filepath + "lena_256x256_yuv420p.yuv";
    std::string yuv422p         = filepath + "lena_256x256_yuv422p.yuv";
    std::string yuv444p         = filepath + "lena_256x256_yuv444p.yuv";
    std::string yuv420p_distort = filepath + "lena_distort_256x256_yuv420p.yuv";

    // 分离YUV420P像素数据中的Y、U、V分量
    simplest_yuv420_split(yuv420p, 256, 256, 1);

    // 分离YUV422P像素数据中的Y、U、V分量
    simplest_yuv422_split(yuv422p, 256, 256, 1);

    // 分离YUV444P像素数据中的Y、U、V分量
    simplest_yuv444_split(yuv444p, 256, 256, 1);

    // 将YUV420P像素数据去掉颜色（变成灰度图）
    simplest_yuv420_gray(yuv420p, 256, 256, 1);

    //  将YUV420P像素数据的亮度减半
    simplest_yuv420_halfy(yuv420p, 256, 256, 1);

    // 将YUV420P像素数据的周围加上边框
    simplest_yuv420_border(yuv420p, 256, 256, 20, 1);

    // 生成YUV420P格式的灰阶测试图
    simplest_yuv420_graybar(640, 360, 0, 255, 10);

    // 计算两个YUV420P像素数据的PSNR
    simplest_yuv420_psnr(yuv420p, yuv420p_distort, 256, 256, 1);

    return 0;
}