# 引入外部函数
include(${ROOT_DIR}/cmake/module.cmake)

# 创建项目
set(ProjectName "yuv")
CreateTarget(${ProjectName} "Exe" "base/image")

# 添加头文件搜索路径
target_include_directories(${ProjectName} PRIVATE
    ${ROOT_DIR}/src
    ${ROOT_DIR}/3rdparty/spdlog/include
)

# 拷贝资源文件
add_custom_command(
    TARGET "${ProjectName}" POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/resources/" "${BUILD_DIR}/bin/resources"
    COMMENT "Copying resources"
)
