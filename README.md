# 音视频



## 环境

- Visual Studio 2022
- CMake 3.15+
- vcpkg


## 下载

```git
git clone --recursive git@github.com:august295/learn_audio_video.git
```


## 编译

```cmake
cmake -B"build" -G"Visual Studio 17 2022"
cmake --build build
```



## 工具

| 名称           | 网址                                      | 作用        |
| -------------- | ----------------------------------------- | ----------- |
| YUView         | https://github.com/IENT/YUView            | 查看 YUV    |
| audacity       | https://github.com/audacity/audacity      | 音频编辑器  |
| H264BSAnalyzer | https://github.com/latelee/H264BSAnalyzer | H264 分析器 |