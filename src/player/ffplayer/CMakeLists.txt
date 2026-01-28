# 引入外部函数
include(${ROOT_DIR}/cmake/module.cmake)

# 创建项目
set(ProjectName "ffplayer")
CreateTarget(${ProjectName} "Exe" "player")

# 添加头文件搜索路径
target_include_directories(${ProjectName} PRIVATE
    ${ROOT_DIR}/src
    ${ROOT_DIR}/3rdparty/spdlog/include
)

# 添加依赖
target_include_directories(${ProjectName} PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(${ProjectName} PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(${ProjectName} PRIVATE ${FFMPEG_LIBRARIES})
target_link_libraries(${ProjectName} PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
