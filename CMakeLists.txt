cmake_minimum_required(VERSION 3.20)

project(rosa)

file(GLOB_RECURSE SOURCES "src/*.cpp")

#set(Boost_USE_STATIC_LIBS        ON)
find_package(imgui CONFIG REQUIRED)
find_package(SFML CONFIG REQUIRED COMPONENTS graphics system window)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(PhysFS REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
#find_package(thor REQUIRED)
#find_package(entt)
#find_package(Boost COMPONENTS di REQUIRED)

#if (_WIN32)                                                                                          
#  add_executable(rosa_exe WIN32 ${SOURCES})
  #set(vcpkg_INCLUDE_DIR ${_VCPKG_INSTALLED_DIR}/x64-linux/include)                                   
#else (_WIN32)                                                                                        
add_library(
  rosa ${SOURCES}
)
                                                                
#endif (_WIN32)

target_compile_features(rosa PRIVATE cxx_std_20)
target_include_directories(rosa PRIVATE
    include/
)
target_include_directories(rosa SYSTEM PUBLIC
    vendor/
    ${PHYSFS_INCLUDE_DIR}
    ${vcpkg_INCLUDE_DIR}
)
target_link_libraries(rosa PRIVATE
  imgui::imgui
  sfml-graphics sfml-system sfml-window
  ImGui-SFML::ImGui-SFML
  ${PHYSFS_LIBRARY}
  ${vcpkg_LIB_DIR}/libthor-s.a
  spdlog::spdlog_header_only
  fmt::fmt-header-only
)

add_subdirectory(testbed)