cmake_minimum_required(VERSION 3.20)

project(rosa)

file(GLOB_RECURSE SOURCES "src/*.cpp")

if (ROSA_PROFILE)
  add_definitions(-DROSA_PROFILE)
endif(ROSA_PROFILE)

#set(Boost_USE_STATIC_LIBS        ON)
find_package(PhysFS REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(stduuid CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(Lua REQUIRED)
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

find_package(imgui CONFIG REQUIRED)

target_precompile_headers(rosa 
  PUBLIC
    include/rosa.hpp
)

target_compile_features(rosa PRIVATE cxx_std_20)
target_include_directories(rosa PRIVATE
    include/
)
target_include_directories(rosa SYSTEM PUBLIC
    vendor/
    ${PHYSFS_INCLUDE_DIR}
    ${vcpkg_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
)
target_link_libraries(rosa PRIVATE
  ${PHYSFS_LIBRARY}
  spdlog::spdlog_header_only
  fmt::fmt-header-only
  yaml-cpp
  stduuid
  lua
  sol2
  ${_VCPKG_INSTALLED_DIR}/x64-linux/lib/libglad.a
  ${_VCPKG_INSTALLED_DIR}/x64-linux/lib/libglfw3.a
  ${_VCPKG_INSTALLED_DIR}/x64-linux/lib/libuuid.a
  X11
  imgui::imgui
)

add_subdirectory(testbed)