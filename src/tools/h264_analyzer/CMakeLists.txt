# 引入外部函数
include(${ROOT_DIR}/cmake/module.cmake)

# 文件
set(HEADER_FILES
)
set(SOURCE_FILES
    main.cpp
    ${ROOT_DIR}/3rdparty/imgui/imgui.cpp
    ${ROOT_DIR}/3rdparty/imgui/imgui_demo.cpp
    ${ROOT_DIR}/3rdparty/imgui/imgui_draw.cpp
    ${ROOT_DIR}/3rdparty/imgui/imgui_tables.cpp
    ${ROOT_DIR}/3rdparty/imgui/imgui_widgets.cpp
    ${ROOT_DIR}/3rdparty/imgui/backends/imgui_impl_glfw.cpp
    ${ROOT_DIR}/3rdparty/imgui/backends/imgui_impl_opengl3.cpp
)

# 文件分类
if(CMAKE_CXX_PLATFORM_ID MATCHES "Windows")
    source_group(TREE ${ROOT_DIR} PREFIX "Header Files" FILES ${HEADER_FILES})
    source_group(TREE ${ROOT_DIR} PREFIX "Source Files" FILES ${SOURCE_FILES})
    source_group(TREE ${ROOT_DIR} PREFIX "Lua Files" FILES ${LUA_FILES})
    source_group(TREE ${ROOT_DIR} PREFIX "Resource Files" FILES ${RESOURCE_FILES})
else()
endif()

# 创建项目
set(ProjectName "h264_analyzer")
add_executable(${ProjectName}
    ${HEADER_FILES} ${SOURCE_FILES}
)
set_property(TARGET ${ProjectName} PROPERTY FOLDER "tools")

# 添加头文件搜索路径
target_include_directories(${ProjectName} PRIVATE
    ${ROOT_DIR}/3rdparty/spdlog/include
    ${ROOT_DIR}/3rdparty/imgui
    ${ROOT_DIR}/3rdparty/imgui/backends
)
set_target_properties(${ProjectName} PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

# 添加依赖
target_link_libraries(${ProjectName} PRIVATE glfw)
target_link_libraries(${ProjectName} PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
target_link_libraries(${ProjectName} PRIVATE ${OPENGL_LIBRARIES})
