#include <string>

#include "pcm.h"

int main(int argc, char* argv[])
{
    std::string filepath = "resources/";
    std::string pcm_16le = filepath + "NocturneNo2inEflat_44.1k_s16le.pcm";
    std::string pcm_drum = filepath + "drum.pcm";

    // 分离PCM16LE双声道音频采样数据的左声道和右声道
    simplest_pcm16le_split(pcm_16le);

    // 将PCM16LE双声道音频采样数据中左声道的音量降一半
    simplest_pcm16le_halfvolumeleft(pcm_16le);

    // 将PCM16LE双声道音频采样数据的声音速度提高一倍
    simplest_pcm16le_doublespeed(pcm_16le);

    // 将PCM16LE双声道音频采样数据转换为PCM8音频采样数据
    simplest_pcm16le_to_pcm8(pcm_16le);

    // 从PCM16LE单声道音频采样数据中截取一部分数据
    simplest_pcm16le_cut_singlechannel(pcm_drum, 2360, 120);

    // 将PCM16LE双声道音频采样数据转换为WAVE格式音频数据
    simplest_pcm16le_to_wave(pcm_16le, 2, 44100);

    return 0;
}