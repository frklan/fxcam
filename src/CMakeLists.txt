set(SOURCE_FILES 
  main.cpp
)

option(BUILD_MACOS_APP "Build MacOS .app bundle" OFF)
  if(BUILD_MACOS_APP)
    set(ICON_NAME "fxcam.icns")
    set(ICON_PATH ${PROJECT_SOURCE_DIR}/res/${ICON_NAME})
    set(${PROJECT_NAME}_ICON ${ICON_PATH})
    set_source_files_properties(${PROJECT_NAME_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    file(COPY ${ICON_PATH} DESTINATION "${PROJECT_NAME}.app/Contents/Resources")
    file(COPY "${CMAKE_SOURCE_DIR}/src/deploy.prototxt" DESTINATION "${PROJECT_NAME}.app/Contents/MacOS")
    file(COPY "${CMAKE_SOURCE_DIR}/src/res10_300x300_ssd_iter_140000_fp16.caffemodel" DESTINATION "${PROJECT_NAME}.app/Contents/MacOS")

    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${ICON_PATH} ${SOURCE_FILES})
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_ICONFILE ${ICON_NAME})
  else()
    add_executable(${PROJECT_NAME}
      ${SOURCE_FILES}
    )
  endif()


target_link_libraries(
  ${PROJECT_NAME} 
  PRIVATE 
    project_options
    project_warnings
  SYSTEM
    extdeps
    spdlog::spdlog
    SDL2::SDL2
    SDL2::SDL2-static SDL2::SDL2main
    opencv::opencv
)
