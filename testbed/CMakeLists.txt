project(rosa_testbed)
file(GLOB_RECURSE SOURCES "*.cpp")
add_executable(rosa_testbed ${SOURCES}) 
set_property(TARGET rosa_testbed PROPERTY OUTPUT_NAME rosa_testbed)

ADD_CUSTOM_COMMAND(
    TARGET rosa_testbed
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar cvf ${CMAKE_CURRENT_BINARY_DIR}/base.pak --format=zip -- .
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/
)

target_compile_features(rosa_testbed PRIVATE cxx_std_20)
target_include_directories(rosa_testbed PRIVATE
    ../include/
)
target_include_directories(rosa_testbed SYSTEM PUBLIC
    #vendor/
    #${PHYSFS_INCLUDE_DIR}
    #${vcpkg_INCLUDE_DIR}
)
target_link_libraries(rosa_testbed PRIVATE
#   imgui::imgui
#   sfml-graphics sfml-system sfml-window
#   ImGui-SFML::ImGui-SFML
#   ${PHYSFS_LIBRARY}
#   ${vcpkg_LIB_DIR}/libthor-s.a
#   spdlog::spdlog_header_only
  rosa
)