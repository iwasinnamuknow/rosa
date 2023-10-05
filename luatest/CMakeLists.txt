project(luatest)
file(GLOB_RECURSE SOURCES "*.cpp")
add_executable(luatest ${SOURCES}) 
set_property(TARGET luatest PROPERTY OUTPUT_NAME luatest)

target_compile_features(luatest PRIVATE cxx_std_20)
# target_include_directories(luatest PRIVATE
#     ../include/
# )
target_include_directories(luatest SYSTEM PUBLIC
    #vendor/
    #${PHYSFS_INCLUDE_DIR}
    ${vcpkg_INCLUDE_DIR}
)
target_link_libraries(luatest PRIVATE
#   imgui::imgui
#   sfml-graphics sfml-system sfml-window
#   ImGui-SFML::ImGui-SFML
#   ${PHYSFS_LIBRARY}
#   ${vcpkg_LIB_DIR}/libthor-s.a
#   spdlog::spdlog_header_only
  #rosa
    lua
    sol2
)