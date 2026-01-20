#include <string>

#include "rgb.h"

int main(int argc, char* argv[])
{
    std::string filepath        = "resources/";
    std::string rgb_cie1931     = filepath + "cie1931_500x500_rgb24.rgb";
    std::string rgb_lena        = filepath + "lena_256x256_rgb24.rgb";

    // 分离RGB24像素数据中的R、G、B分量
    simplest_rgb24_split(rgb_cie1931, 500, 500, 1);

    // 将RGB24格式像素数据封装为BMP图像
    simplest_rgb24_to_bmp(rgb_lena, 256, 256);

    // 将RGB24格式像素数据转换为YUV420P格式像素数据
    simplest_rgb24_to_yuv420(rgb_lena, 256, 256, 1);

    // 生成RGB24格式的彩条测试图
    simplest_rgb24_colorbar(640, 360);

    return 0;
}