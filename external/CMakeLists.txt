
project(extdeps)

include(../cmake/PreventInSourceBuilds.cmake)

add_library(${PROJECT_NAME}
  ImGui/imgui_impl_sdlrenderer.cpp
  ImGui/imgui_impl_sdl.cpp
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)

target_link_libraries(
  ${PROJECT_NAME} 
  SYSTEM
    spdlog::spdlog
    SDL2::SDL2
    SDL2::SDL2-static SDL2::SDL2main
    opencv::opencv
)
#${PROJECT_NAME}
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${imgui_BINARY_DIR})
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ImGui)
