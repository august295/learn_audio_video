# 音视频基础
add_subdirectory(base/yuv)
add_subdirectory(base/rgb)
add_subdirectory(base/h264)

add_subdirectory(base/pcm)
add_subdirectory(base/aac)

add_subdirectory(base/flv)

# 三方库
add_subdirectory(sdl2)

# 工具
add_subdirectory(tools/h264_analyzer)

# player
add_subdirectory(player/ffplayer)