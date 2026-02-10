# 引入外部函数
include(${ROOT_DIR}/cmake/module.cmake)

# 创建项目
set(ProjectName "14animated_sprites")
CreateTarget(${ProjectName} "Exe" "sdl2")

# 添加头文件搜索路径
target_include_directories(${ProjectName} PRIVATE
    ${ROOT_DIR}/src
    ${ROOT_DIR}/3rdparty/spdlog/include
)
set_target_properties(${ProjectName} PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

# 添加依赖
target_link_libraries(${ProjectName} PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
)

# 拷贝资源文件
add_custom_command(
    TARGET "${ProjectName}" POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/resources/" "${BUILD_DIR}/bin/resources"
    COMMENT "Copying resources"
)
