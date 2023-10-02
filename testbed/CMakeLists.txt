file(GLOB_RECURSE SOURCES "*.cpp")
add_executable(rosa_exe ${SOURCES}) 
set_property(TARGET rosa_exe PROPERTY OUTPUT_NAME rosa)

target_compile_features(rosa_exe PRIVATE cxx_std_20)
target_include_directories(rosa_exe PRIVATE
    ../include/
)
target_include_directories(rosa_exe SYSTEM PUBLIC
    #vendor/
    #${PHYSFS_INCLUDE_DIR}
    #${vcpkg_INCLUDE_DIR}
)
target_link_libraries(rosa_exe PRIVATE
#   imgui::imgui
#   sfml-graphics sfml-system sfml-window
#   ImGui-SFML::ImGui-SFML
#   ${PHYSFS_LIBRARY}
#   ${vcpkg_LIB_DIR}/libthor-s.a
#   spdlog::spdlog_header_only
  rosa
)