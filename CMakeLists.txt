cmake_minimum_required(VERSION 3.16)
set(CMAKE_TOOLCHAIN_FILE "./vendor/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(rosa)

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(Boost_INCLUDE_DIR ${_VCPKG_INSTALLED_DIR}/x64-linux/include)
#set(Boost_USE_STATIC_LIBS        ON)
find_package(imgui CONFIG REQUIRED)
find_package(SFML CONFIG REQUIRED COMPONENTS graphics system window)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(PhysFS REQUIRED)
#find_package(entt)
#find_package(Boost COMPONENTS di REQUIRED)

if (_WIN32)                                                                                          
  add_executable(rosa_exe WIN32 ${SOURCES})                                                          
else (_WIN32)                                                                                        
  add_executable(rosa_exe ${SOURCES})                                                                
endif (_WIN32)

set_property(TARGET rosa_exe PROPERTY OUTPUT_NAME rosa)

target_compile_features(rosa_exe PRIVATE cxx_std_20)
target_include_directories(rosa_exe PRIVATE
    include/
    vendor/
    ${PHYSFS_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
)
target_link_libraries(rosa_exe PRIVATE
  imgui::imgui
  sfml-graphics sfml-system sfml-window
  ImGui-SFML::ImGui-SFML
  ${PHYSFS_LIBRARY}
)