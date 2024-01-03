cmake_minimum_required(VERSION 3.20)

project(rosa)

add_subdirectory(vendor/soloud/contrib)

file(GLOB_RECURSE SOURCES "src/*.cpp")

if (ROSA_PROFILE)
  add_definitions(-DTRACY_ENABLE)
endif(ROSA_PROFILE)

#set(Boost_USE_STATIC_LIBS        ON)
find_package(PhysFS REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(stduuid CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(Lua REQUIRED)
find_package(lodepng CONFIG REQUIRED)
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
find_package(Tracy CONFIG REQUIRED)

target_precompile_headers(rosa 
  PUBLIC
    include/rosa_pch.hpp
)

target_compile_features(rosa PRIVATE cxx_std_20)
target_include_directories(rosa PRIVATE
    include/
)
target_compile_definitions(rosa PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")
target_include_directories(rosa SYSTEM PUBLIC
    vendor/
    ${PHYSFS_INCLUDE_DIR}
    ${vcpkg_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
)

if(WIN32)
  set(VCPKG_LIB ${_VCPKG_INSTALLED_DIR}/x64-windows-static-md/lib/)
  set(OPENGL_SHIT
    ${VCPKG_LIB}/glad.lib
    ${VCPKG_LIB}/glfw3.lib
  )
else()
  set(VCPKG_LIB ${_VCPKG_INSTALLED_DIR}/x64-linux/lib/)
  set(OPENGL_SHIT
    ${VCPKG_LIB}/libglad.a
    ${VCPKG_LIB}/libglfw3.a
  )
endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
  list(GET PHYSFS_LIBRARY 3 FINAL_PHYSFS_LIB)
else()
  list(GET PHYSFS_LIBRARY 1 FINAL_PHYSFS_LIB)
endif()

set (GENERAL_LIBS
  ${FINAL_PHYSFS_LIB}
  spdlog::spdlog_header_only
  fmt::fmt-header-only
  yaml-cpp
  stduuid
  lua
  sol2
  ${OPENGL_SHIT}
  imgui::imgui
  soloud
  lodepng
)

# set (WINDOWS_LIBS)

set (LINUX_LIBS
  X11
)

set (PROFILE_LIBS
  Tracy::TracyClient
)

set (FINAL_LIBS
  ${GENERAL_LIBS}
)

if (ROSA_PROFILE)
  set (FINAL_LIBS
    ${FINAL_LIBS}
    ${PROFILE_LIBS}
  )
endif (ROSA_PROFILE)

if(WIN32)
  set(FINAL_LIBS
    ${FINAL_LIBS}
    ${WINDOWS_LIBS}
  )
else()
  set(FINAL_LIBS
    ${FINAL_LIBS}
    ${LINUX_LIBS}
  )
endif()
  
target_link_libraries(rosa PRIVATE
  ${FINAL_LIBS}
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif ()

add_subdirectory(examples)
add_subdirectory(docs)
add_subdirectory(tests)